#include <Arduino.h>
#include <Preferences.h>
#include "M5StickCPlus.h"
#include "Ticker.h"

#include "main_programs.h"
#include "remote_setting_modes.h"
#include "preference_modes.h"

#include "CanonBLERemote.h"
#include "display.h"
#include "led.h"
#include "remote_preferences.h"
#include "timeLapse_management.h"
#include "remote_management.h"

#define LOG_LOCAL_LEVEL ESP_LOG_ERROR
#include <esp_log.h>
#include <esp32-hal-log.h>

const String m_nameRemote = "BR-M5";
CanonBLERemote canonBle(m_nameRemote);

Display M5_display(&M5.Lcd, m_nameRemote);

const String m_prefsNamespace = "BR-M5-remote";
Preferences m_preferences;
RemotePreferences *m_pRemotePrefs;

Ticker m_blinker;

TimeLapse *m_pTimelapse;
Remote *m_pRemote;
Led m_led;
bool m_checkButtonState = true;

uint32_t m_sleepCount;
uint32_t m_sleepDuration = 1000 * 60;
bool m_disconnecting = false;

MainPrograms currentMainProgram;

const uint32_t BTN_DEBOUNCE_MS = 100;

enum Pages
{
    MAIN_MODES,
    SETTINGS_MODE,
    SETTINGS_REMOTE,
    SETTINGS_TIMELAPSE,
    WAITING,
    DISCONNECTING,
    RECONNECTING,
} m_currentPage;

PreferenceModes m_currentPrefMode = PreferenceModes::DISPLAY_NEG;
bool m_deviceBeep = true;
bool m_deviceLed = true;

RemoteSettingModes m_currentRemoteSettingPage = RemoteSettingModes::DELAY;

bool m_btnADebounceStart = false;
bool m_btnBDebounceStart = false;

bool m_checkASecondClick = false;
bool m_checkBSecondClick = false;

void blink()
{
    digitalWrite(M5_LED, !digitalRead(M5_LED));
}

void set_current_program_display()
{
    switch (currentMainProgram)
    {
    case MainPrograms::REMOTE:
        M5_display.setRemoteMenuScreen(m_pRemote->getDelay(),
                                       m_pRemote->getShots(),
                                       m_pRemote->getInterval(),
                                       "Ready");
        break;

    case MainPrograms::TIMELAPSE:
        M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(),
                                          "Ready for timelapse");
        break;

    case MainPrograms::PREFERENCES:
        m_currentPrefMode = PreferenceModes::DISPLAY_NEG;
        M5_display.setPreferencesScreen(m_currentPrefMode, m_deviceLed, m_deviceBeep);
        break;

    default:
        break;
    }
}

void setRemoteSettingDisplay()
{
    M5_display.setRemoteMenuSettings(m_pRemote->getDelay(),
                                     m_pRemote->getShots(),
                                     m_pRemote->getInterval(),
                                     m_currentRemoteSettingPage);
}

void beep(int duration)
{
    if (m_deviceBeep)
    {
        M5.Beep.setBeep(4000, duration);
        M5.Beep.beep();
    }

    if (m_deviceLed)
    {
        m_led.setDuration(duration);
        m_led.on();
    }
}

void trigger()
{
    canonBle.trigger();
}

void remoteFinished()
{
    M5_display.setRemoteMenuScreen(m_pRemote->getDelay(),
                                   m_pRemote->getShots(),
                                   m_pRemote->getInterval(),
                                   "Ready");
}

void loop()
{
    // Update buttons state
    M5.update();
    m_led.update();

    bool btnAPressDebounced = false;
    bool btnBPressDebounced = false;

    bool btnADoublePress = false;
    bool btnBDoublePress = false;

    bool btnALongPress = false;
    bool btnBLongPress = false;

    // checkButtonState used to prevent making an action when a button is released after a long press
    if (M5.BtnB.pressedFor(700) && m_checkButtonState)
    {
        m_checkButtonState = false;
        btnBLongPress = true;
    }

    if (M5.BtnA.pressedFor(700) && m_checkButtonState)
    {
        m_checkButtonState = false;
        btnALongPress = true;
    }

    if (M5.BtnA.wasReleased() && m_checkButtonState)
    {
        m_btnADebounceStart = true;

        if (m_checkASecondClick)
        {
            m_checkASecondClick = false;
            btnADoublePress = true;
            m_btnADebounceStart = false;
        }
        else
        {
            m_checkASecondClick = true;
        }
    }

    // btn A debounce
    if (m_btnADebounceStart && millis() - M5.BtnA.lastChange() > BTN_DEBOUNCE_MS && m_checkButtonState)
    {
        btnAPressDebounced = true;
        m_btnADebounceStart = false;
        m_checkASecondClick = false;
        m_sleepCount = millis() + m_sleepDuration;
    }

    if (M5.BtnB.wasReleased() && m_checkButtonState)
    {
        m_btnBDebounceStart = true;

        if (m_checkBSecondClick)
        {
            m_checkBSecondClick = false;
            btnBDoublePress = true;
            m_btnBDebounceStart = false;
        }
        else
        {
            m_checkBSecondClick = true;
        }
    }

    // btn B debounce
    if (m_btnBDebounceStart && millis() - M5.BtnB.lastChange() > BTN_DEBOUNCE_MS && m_checkButtonState)
    {
        btnBPressDebounced = true;
        m_btnBDebounceStart = false;

        m_checkBSecondClick = false;
        m_sleepCount = millis() + m_sleepDuration;
    }

    if (currentMainProgram != MainPrograms::TIMELAPSE &&
        canonBle.isConnected() && millis() > m_sleepCount && !m_disconnecting) // going to sleep mode to release camera and remote
    {
        log_v("Going to sleep, disconnecting camera...");
        m_disconnecting = true;
        m_currentPage = DISCONNECTING; // using current page will lock other interactions
        canonBle.disconnect();
    }

    if (!canonBle.isConnected() && (m_currentPage != DISCONNECTING &&
                                    m_currentPage != WAITING &&
                                    m_currentPage != RECONNECTING &&
                                    currentMainProgram != MainPrograms::PREFERENCES)) // camera disconnected
    {
        log_v("Camera disconnected");
        m_currentPage = DISCONNECTING; // using current page will lock other interactions
    }

    switch (m_currentPage)
    {
    case DISCONNECTING:
        if (!canonBle.isConnected())
        {
            log_d("Camera sleep or disconnected!, change screen");
            M5.Axp.ScreenBreath(RemotePreferences::DEFAULT_LOW_BRIGHTNESS);
            m_currentPage = WAITING;
            m_disconnecting = false;
            M5_display.setInitScreen(2);
        }
        break;

    case RECONNECTING:
        if (canonBle.isConnected())
        {
            log_v("Camera reconnected!");
            m_currentPage = MAIN_MODES;
            set_current_program_display();

            M5.Axp.ScreenBreath(m_pRemotePrefs->getBrightness());
        }
        break;

    case WAITING:
        if (btnAPressDebounced || btnBPressDebounced)
        {
            log_d("Reconnecting camera... change screen");
            M5_display.setInitScreen(1);

            canonBle.forceCameraConnection(); // to wake up the camera from sleep
            m_currentPage = RECONNECTING;
        }
        break;

    case SETTINGS_MODE:
        if (btnBLongPress || btnAPressDebounced) // go back to previous mode
        {
            m_currentPage = MAIN_MODES;
            set_current_program_display();
            break;
        }

        // // update mode settings
        // if (btnAPressDebounced)
        // {
        //     current_page = MAIN_MODES;
        //     set_current_program_display();
        //     break;
        // }

        if (btnBPressDebounced)
        {
            currentMainProgram++;
            if (currentMainProgram == MainPrograms::REMOTE || currentMainProgram == MainPrograms::TIMELAPSE) // ignore preferences mode
            {
                m_pRemotePrefs->setShootingMode(currentMainProgram);
            }
            M5_display.setSettingsModeScreen(currentMainProgram);
            break;
        }
        break;

    case SETTINGS_REMOTE:
        if (btnALongPress)
        {
            m_currentPage = MAIN_MODES;
            M5_display.setRemoteMenuScreen(m_pRemote->getDelay(),
                                           m_pRemote->getShots(),
                                           m_pRemote->getInterval(),
                                           "Ready");
            break;
        }

        if (btnBLongPress)
        {
            m_currentRemoteSettingPage++;
            setRemoteSettingDisplay();
            break;
        }

        // update remote settings
        if (M5.BtnA.wasReleased() && m_checkButtonState)
        {
            switch (m_currentRemoteSettingPage)
            {
            case RemoteSettingModes::DELAY:
                m_pRemote->decDelay();
                break;

            case RemoteSettingModes::SHOTS:
                m_pRemote->decShots();
                break;

            case RemoteSettingModes::INTERVAL:
                if (m_pRemote->getShots() > 1)
                {
                    m_pRemote->decInterval();
                }
                break;

            default:
                break;
            }

            setRemoteSettingDisplay();
            break;
        }

        if (M5.BtnB.wasReleased() && m_checkButtonState)
        {
            switch (m_currentRemoteSettingPage)
            {
            case RemoteSettingModes::DELAY:
                m_pRemote->incDelay();
                break;

            case RemoteSettingModes::SHOTS:
                m_pRemote->incShots();
                break;

            case RemoteSettingModes::INTERVAL:
                if (m_pRemote->getShots() > 1)
                {
                    m_pRemote->incInterval();
                }
                break;

            default:
                break;
            }

            setRemoteSettingDisplay();
            break;
        }
        break;

    case SETTINGS_TIMELAPSE:
        if (btnALongPress)
        {
            m_currentPage = MAIN_MODES;
            M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(), "Ready for timelapse");
            break;
        }

        // update timelapse settings
        if (M5.BtnA.wasReleased() && m_checkButtonState)
        {
            m_pTimelapse->decDelay();
            M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(), "Setting interval");
            break;
        }

        if (M5.BtnB.wasReleased() && m_checkButtonState)
        {
            m_pTimelapse->incDelay();
            M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(), "Setting interval");
            break;
        }

        break;

    case MAIN_MODES:
        switch (currentMainProgram)
        {
        case MainPrograms::REMOTE:
            if (btnALongPress)
            {
                log_v("focusing...");
                canonBle.focus();
                break;
            }

            if (btnBLongPress)
            {
                m_currentPage = SETTINGS_REMOTE;
                setRemoteSettingDisplay();
                break;
            }

            if (btnADoublePress)
            {
                log_i("Double A");
                m_pRemote->startNow();
                break;
            }

            // Double click btn B, go to mode settings
            if (btnBDoublePress)
            {
                m_currentPage = SETTINGS_MODE;
                M5_display.setSettingsModeScreen(MainPrograms::REMOTE);
                break;
            }

            // update remote shooting
            m_pRemote->update();

            if (btnAPressDebounced)
            {
                if (m_pRemote->startOnOFF())
                {
                    log_v("Start remote");
                    M5_display.setRemoteMenuScreen(m_pRemote->getDelay(),
                                                   m_pRemote->getShots(),
                                                   m_pRemote->getInterval(),
                                                   "Shooting");
                }
                else
                {
                    log_v("Stop remote");
                    M5_display.setRemoteMenuScreen(m_pRemote->getDelay(),
                                                   m_pRemote->getShots(),
                                                   m_pRemote->getInterval(),
                                                   "Ready");
                }

                break;
            }
            break;

        case MainPrograms::TIMELAPSE:
            if (btnBLongPress)
            {
                m_currentPage = SETTINGS_TIMELAPSE;
                M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(), "Setting interval");
                break;
            }

            if (btnALongPress)
            {
                log_v("focusing...");
                canonBle.focus();
                break;
            }

            if (btnBDoublePress)
            {
                m_currentPage = SETTINGS_MODE;
                M5_display.setSettingsModeScreen(MainPrograms::TIMELAPSE);
                break;
            }

            // update timelapse shooting

            // Update timelapse logic
            m_pTimelapse->update();

            // Stop or start timelapse
            if (btnAPressDebounced)
            {
                if (canonBle.forceCameraConnection())
                {
                    if (m_pTimelapse->recordingOnOFF())
                    {
                        log_v("Start timelapse");
                        M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(),
                                                          "Shooting timelapse");
                    }
                    else
                    {
                        log_v("Stop timelapse");
                        M5_display.setTimelapseMenuScreen(m_pTimelapse->getInterval(),
                                                          "Ready for timelapse");
                    }
                }
                break;
            }

        case MainPrograms::PREFERENCES:
            // Double click btn B, go to mode settings
            if (btnBDoublePress)
            {
                m_currentPage = SETTINGS_MODE;
                M5_display.setSettingsModeScreen(MainPrograms::PREFERENCES);
                break;
            }

            if (btnBPressDebounced)
            {
                m_currentPrefMode++;
                M5_display.setPreferencesScreen(m_currentPrefMode, m_deviceLed, m_deviceBeep);
            }

            if (btnAPressDebounced)
            {
                switch (m_currentPrefMode)
                {
                case PreferenceModes::DISPLAY_NEG:
                {
                    bool neg = M5_display.toggleDisplay();
                    m_pRemotePrefs->setDisplayNeg(neg);
                }
                break;

                case PreferenceModes::LED:
                    m_deviceLed = !m_deviceLed;
                    m_pRemotePrefs->setDeviceLed(m_deviceLed);
                    break;
                case PreferenceModes::BUZZER:
                    m_deviceBeep = !m_deviceBeep;
                    m_pRemotePrefs->setDeviceBuzzer(m_deviceLed);
                    break;
                }

                M5_display.setPreferencesScreen(m_currentPrefMode, m_deviceLed, m_deviceBeep);
            }

            break;
        }

        break;

    default:
        break;
    }

    if (M5.BtnA.wasReleased())
    {
        m_checkButtonState = true;
    }

    if (M5.BtnB.wasReleased())
    {
        m_checkButtonState = true;
    }

    delay(20);
}

int8_t getBatteryLevel(void)
{
    // double bat_charge_p = 100 * ((M5.Axp.GetBatVoltage() - 3.0) / 1.16); // 1.07, 1.1, 1.2...  may need more tuning
    double bat_charge_p = ((M5.Axp.GetBatVoltage() - 3.27) / (4.2 - 3.27)) * 100;

    log_v("Batt level: %.3f", bat_charge_p);

    if (bat_charge_p > 100)
    {
        bat_charge_p = 100;
    }
    else if (bat_charge_p < 0)
    {
        bat_charge_p = 0;
    }

    return (int8_t)round(bat_charge_p);
}

void setBatteryLevel()
{
    M5_display.setBatteryInfos(getBatteryLevel(), M5.Axp.GetBatState());
    switch (m_currentPage)
    {
    case MAIN_MODES:
        set_current_program_display();
        break;

    case SETTINGS_MODE:
    case SETTINGS_REMOTE:
    case SETTINGS_TIMELAPSE:
        setRemoteSettingDisplay();
        break;

    default:
        break;
    }
}

void setup()
{
    esp_log_level_set("*", ESP_LOG_INFO);

    M5.begin(true, true, true);

    m_preferences.begin(m_prefsNamespace.c_str(), false);

    m_pRemotePrefs = new RemotePreferences(&m_preferences);

    m_deviceLed = m_pRemotePrefs->getDeviceLed();
    m_deviceBeep = m_pRemotePrefs->getDeviceBuzzer();

    m_pRemote = new Remote(m_pRemotePrefs, &beep, &trigger, &remoteFinished);
    m_pTimelapse = new TimeLapse(m_pRemotePrefs, &trigger);

    currentMainProgram = m_pRemotePrefs->getShootingMode();
    m_currentPage = MAIN_MODES;

    int do_pair = M5.BtnA.isPressed() ? 0 : 1;

    log_v("do pair %d", do_pair);

    M5.Axp.ScreenBreath(m_pRemotePrefs->getBrightness());
    delay(100);
    M5_display.setInitScreen(do_pair);

    // M5.Beep.setVolume(1);

    canonBle.init();

    delay(500);

    // Pairing
    if (do_pair == 0)
    {
        m_blinker.attach_ms(200, blink);
        // pair() function should be called only when you want to pair with the new camera.
        // After paired, the pair() function should not be called.
        do
        {
            log_v("Pairing...");
        } while (!canonBle.pair(10));

        m_blinker.detach();
        digitalWrite(M5_LED, HIGH);
    }

    delay(500);
    log_v("Setup Done");

    do
    {
        log_v("Waiting for camera...");
    } while (!canonBle.forceCameraConnection());

    M5_display.setAddress(canonBle.getPairedAddressString());
    // M5_display.set_main_menu_screen(m_pTimelapse.get_interval(), "Ready for single shot");
    set_current_program_display();

    m_blinker.attach_ms(30000, setBatteryLevel);
    m_sleepCount = millis() + m_sleepDuration;
}