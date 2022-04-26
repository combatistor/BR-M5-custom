#include "CanonBLERemote.h"
#include <Arduino.h>
#include "Ticker.h"
#include "M5StickCPlus.h"
#include "Display.h"
#include "Led.h"
#include "TimeLapse_Management.h"
#include "Remote_Management.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"
#include <esp32-hal-log.h>

String name_remote = "BR-M5";
CanonBLERemote canon_ble(name_remote);

Display M5_display(&M5.Lcd, name_remote);

Ticker blinker;

TimeLapse timelapse(600);
Remote remote;
LED led;
bool check_button_state = true;

uint32_t m_sleep_count;
uint32_t m_sleep_duration = 1000 * 60;
bool disconnecting = false;

enum ShootingModes
{
    Shooting_remote,
    Shooting_timelapse
} current_shooting_mode;

enum Pages
{
    Shooting,
    Settings_mode,
    Settings_remote,
    Settings_timelapse,
    Sleep,
    Disconnecting,
    Reconnecting,
} current_page;

int current_remote_setting_page = 0; // 0 for delay, 1 for shots, 2 for interval

bool check_second_click = false;

bool connection_changed = false;

void blink(){
    digitalWrite(M5_LED, !digitalRead(M5_LED));
}

void set_current_mode_display()
{
    switch (current_shooting_mode)
    {
    case Shooting_remote:
        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready", -1);
        break;

    case Shooting_timelapse:
        M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Ready for timelapse");
        break;

    default:
        break;
    }
}

void set_remote_setting_display()
{
    M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Settings", current_remote_setting_page);
}

void beep(int duration)
{
    M5.Beep.setBeep(4000, duration);
    M5.Beep.beep();

    led.setDuration(duration);
    led.on();
}

void trigger()
{
    canon_ble.trigger();
}

void remote_finished()
{
    M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready", -1);
}

void change_remote_setting_page()
{
    if (current_remote_setting_page < 2)
    {
        current_remote_setting_page++;
    }
    else
    {
        current_remote_setting_page = 0;
    }
}

void loop()
{
    // Update buttons state
    M5.update();
    led.update();

    bool btnALongPress = false;
    bool btnBLongPress = false;

    bool btnAPressed = false;
    bool btnBPressed = false;

    bool btnBDoublePress = false;
    
    // check_button_state used to prevent making an action when a button is released after a long press 
    if(M5.BtnB.pressedFor(700) && check_button_state)
    {
        check_button_state = false;
        btnBLongPress = true;
    }

    if(M5.BtnA.pressedFor(700) && check_button_state)
    {
        check_button_state = false;
        btnALongPress = true;
    }

    if(M5.BtnA.wasReleased() && check_button_state)
    {
        btnAPressed = true;

        m_sleep_count = millis() + m_sleep_duration;
    }

    if(M5.BtnB.wasReleased() && check_button_state){
        btnBPressed = true;

        if (check_second_click)
        {
            check_second_click = false;
            btnBDoublePress = true;
        }
        else
        {
            check_second_click = true;
        }

        m_sleep_count = millis() + m_sleep_duration;
    }
    
    if(current_shooting_mode != Shooting_timelapse && 
        canon_ble.isConnected() && millis() > m_sleep_count && !disconnecting) // going to sleep mode to release camera and remote
    {
        Serial.println("Going to sleep, disconnecting camera...");
        disconnecting = true;
        current_page = Disconnecting; //using current page will lock other interactions
        canon_ble.disconnect();
    }

    switch (current_page)
    {
    case Disconnecting:
        if(!canon_ble.isConnected())
        {
            Serial.println("Camera disconnected!, change screen");
            // M5.Axp.ScreenBreath(8);
            current_page = Sleep;
            disconnecting = false;
            M5_display.set_init_screen(2);
        }
    break;

    case Reconnecting: 
        if(canon_ble.isConnected())
        {
            Serial.println("Camera reconnected!");
            current_page = Shooting;
            set_current_mode_display();

            // canon_ble.trigger();
            // M5.Axp.ScreenBreath(9);
        }
    break;

    case Sleep:
        if(btnAPressed || btnBPressed)
        {
            Serial.println("Reconnecting camera... change screen");
            M5_display.set_init_screen(1);

            canon_ble.forceCameraConnection(); // to wake up the camera from sleep
            current_page = Reconnecting;
        }
    break;

    case Settings_mode:
        if (btnBLongPress) // go back to previous mode
        {
            current_page = Shooting;
            set_current_mode_display();
            break;
        }

        // update mode settings
        if (btnAPressed)
        {
            current_page = Shooting;
            set_current_mode_display();
            break;
        }

        if (btnBPressed)
        {
            if (current_shooting_mode == Shooting_remote)
            {
                current_shooting_mode = Shooting_timelapse;
            }
            else
            {
                current_shooting_mode = Shooting_remote;
            }
            M5_display.set_settings_mode_screen((current_shooting_mode == Shooting_remote) ? 0 : 1);
            break;
        }

        break;

    case Settings_remote:
        if (btnALongPress)
        {
            current_page = Shooting;
            M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready", -1);
            break;
        }

        if (btnBLongPress)
        {
            change_remote_setting_page();
            set_remote_setting_display();
            break;
        }

        //update remote settings
        if (btnAPressed)
        {
            switch (current_remote_setting_page)
            {
            case 0:
                remote.Remote_decDelay();
                break;

            case 1:
                remote.Remote_decShots();
                break;

            case 2:
                if (remote.get_shots() > 1)
                {
                    remote.Remote_decInterval();
                }
                break;

            default:
                break;
            }

            set_remote_setting_display();
            break;
        }
        
        if (btnBPressed)
        {
            switch (current_remote_setting_page)
            {
            case 0:
                remote.Remote_incDelay();
                break;

            case 1:
                remote.Remote_incShots();
                break;

            case 2:
                if (remote.get_shots() > 1)
                {
                    remote.Remote_incInterval();
                }
                break;

            default:
                break;
            }

            set_remote_setting_display();
            break;
        }
        
        break;

    case Settings_timelapse:
        if (btnALongPress)
        {
            current_page = Shooting;
            M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Ready for timelapse");
            break;
        }

        //update timelapse settings
        if (M5.BtnA.wasReleased() && check_button_state)
        {
            timelapse.TimeLapse_decDelay();
            M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Setting interval");
            break;
        }
        
        if (M5.BtnB.wasReleased() && check_button_state)
        {
            timelapse.TimeLapse_incDelay();
            M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Setting interval");
            break;
        }
        
        break;

    case Shooting:
        // if(!canon_ble.isConnected())
        // {
        //     M5_display.set_init_screen(2);
        //     connection_changed = true;
        //     delay(1000);
        // } 
        // else 
        // {
            if(connection_changed)
            {
                M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready", -1);
                connection_changed = false;
            }
        
            switch (current_shooting_mode)
            {
            case Shooting_remote:
                if (btnBLongPress)
                {
                    current_page = Settings_remote;
                    set_remote_setting_display();
                    break;
                }

                if (btnALongPress)
                {
                    Serial.println("focus...");
                    canon_ble.focus();
                    break;
                }

                // Double click btn B, go to mode settings
                if (btnBDoublePress)
                {
                    current_page = Settings_mode;
                    M5_display.set_settings_mode_screen(0);
                    break;
                }

                //update remote shooting
                remote.Remote_Update();

                if (btnAPressed)
                {
                    if (remote.Start_OnOFF())
                    {
                        Serial.println("Start remote");
                        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Shooting", -1);
                    }
                    else
                    {
                        Serial.println("Stop remote");
                        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready", -1);
                    }
                
                    break;
                    
                }

                break;

            case Shooting_timelapse:
                if (btnBLongPress)
                {
                    current_page = Settings_timelapse;
                    M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Setting interval");
                    break;
                }

                if (btnALongPress)
                {
                    Serial.println("focus...");
                    canon_ble.focus();
                    break;
                }

                if (btnBDoublePress)
                {
                    current_page = Settings_mode;
                    M5_display.set_settings_mode_screen(1);
                    break;
                }

                //update timelapse shooting

                // Update timelapse logic
                timelapse.TimeLapse_Update();

                // Stop or start timelapse
                if (btnAPressed)
                {
                    if(canon_ble.forceCameraConnection())
                    {
                        if (timelapse.Recording_OnOFF())
                        {
                            Serial.println("Start timelapse");
                            M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Shooting timelapse");
                        }
                        else
                        {
                            Serial.println("Stop timelapse");
                            M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Ready for timelapse");
                        }
                    }
                    break;
                }
                
                break;
            }
        // }

        break;

    default:
        break;
    }

    if(M5.BtnA.wasReleased()){
        check_button_state = true;
    }

    if(M5.BtnB.wasReleased()){
        check_button_state = true;
    }

    if (check_second_click && millis() - M5.BtnB.lastChange() > 200)
    {
        // Single click btn B
        check_second_click = false;
    }

    delay(10);
}

int8_t getBatteryLevel(void)
{
    double bat_charge_p = 100 * ((M5.Axp.GetBatVoltage() - 3.0) / 1.16); // 1.07, 1.1, 1.2...  may need more tuning
    Serial.println(bat_charge_p);

    if(bat_charge_p > 100)
    {
        bat_charge_p = 100;
    } else if(bat_charge_p < 0)
    {
        bat_charge_p = 0;
    }

    return (int8_t) round(bat_charge_p);
}

void setBatteryLevel()
{
    M5_display.setBatteryInfos(getBatteryLevel(), M5.Axp.GetBatState());
    switch(current_page)
    {
        case Shooting: 
            set_current_mode_display();
        break;

        case Settings_mode:
        case Settings_remote:
        case Settings_timelapse:
            set_remote_setting_display();
        break;

        default:
        break;
    }
}

void setup()
{
    esp_log_level_set("*", ESP_LOG_INFO);

    M5.begin(true, true, true);
    current_shooting_mode = Shooting_remote;
    // current_shooting_mode = Shooting_timelapse;
    current_page = Shooting;

    int do_pair = M5.BtnA.isPressed() ? 0 : 1;

    M5.Axp.ScreenBreath(9);
    delay(100);
    M5_display.set_init_screen(do_pair);

    // M5.Beep.setVolume(1);

    timelapse.set_callbacks(&trigger);
    remote.set_callbacks(&beep, &trigger, &remote_finished);

    canon_ble.init();

    delay(500);

    // Pairing
    if (do_pair == 0)
    {
        blinker.attach_ms(200, blink);
        // pair() function should be called only when you want to pair with the new camera.
        // After paired, the pair() function should not be called.
        do
        {
            Serial.println("Pairing...");
        } while (!canon_ble.pair(10));

        blinker.detach();
        digitalWrite(M5_LED, HIGH);
    }

    delay(500);
    Serial.println("Setup Done");

    do
    {
        Serial.println("Waiting for camera...");
    } while (!canon_ble.forceCameraConnection());

    M5_display.set_address(canon_ble.getPairedAddressString());
    // M5_display.set_main_menu_screen(timelapse.get_interval(), "Ready for single shot");
    set_current_mode_display();

    blinker.attach_ms(30000, setBatteryLevel);
    m_sleep_count = millis() + m_sleep_duration;
}