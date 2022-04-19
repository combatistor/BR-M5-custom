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
    Settings_timelapse
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
        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready");
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
    switch (current_remote_setting_page)
    {
    case 0:
        M5_display.set_settings_remote_delay_screen(remote.get_delay());
        break;

    case 1:
        M5_display.set_settings_remote_shots_screen(remote.get_shots());
        break;

    case 2:
        M5_display.set_settings_remote_interval_screen(remote.get_shots(), remote.get_interval());
        break;

    default:
        break;
    }
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
    M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready");
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
    M5.Beep.update();
    led.update();

    bool btnALongPress = false;
    bool btnBLongPress = false;

    bool btnAPressed = false;
    bool btnBPressed = false;

    bool btnBDoublePress = false;
    
    // check_button_state used to prevent making an action when a button is released after a long press 
    if(M5.BtnB.pressedFor(700) && check_button_state){
        check_button_state = false;
        btnBLongPress = true;
    }

    if(M5.BtnA.pressedFor(700) && check_button_state){
        check_button_state = false;
        btnALongPress = true;
    }

    if(M5.BtnA.wasReleased() && check_button_state){
        btnAPressed = true;
    }

    if(M5.BtnB.wasReleased() && check_button_state){
        btnBPressed = true;

        if (check_second_click)
        {
            // Double click btn B, go to mode settings
            check_second_click = false;
            btnBDoublePress = true;
        }
        else
        {
            check_second_click = true;
        }
    }

    switch (current_page)
    {
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
            M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready");
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
        if(!canon_ble.forceCameraConnection())
        {
            M5_display.set_init_screen(2);
            connection_changed = true;
            delay(1000);
        } 
        else 
        {
            if(connection_changed)
            {
                M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready");\
                connection_changed = false;
            }
        
            switch (current_shooting_mode)
            {
            case Shooting_remote:
                if (btnBLongPress)
                {
                    M5.Lcd.setRotation(0);

                    current_page = Settings_remote;
                    set_remote_setting_display();
                    break;
                }
                if (btnALongPress)
                {
                    canon_ble.focus();
                    break;
                }

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
                        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Shooting");
                    }
                    else
                    {
                        Serial.println("Stop remote");
                        M5_display.set_remote_menu_screen(remote.get_delay(), remote.get_shots(), remote.get_interval(), "Ready");
                    }
                
                    break;
                    
                }

                break;

            case Shooting_timelapse:
                if (btnBLongPress)
                {
                    M5.Lcd.setRotation(0);
                    current_page = Settings_timelapse;
                    M5_display.set_timelapse_menu_screen(timelapse.get_interval(), "Setting interval");
                    break;
                }

                if (btnALongPress)
                {
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
        }

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

void setup()
{
    Serial.begin(115200);

    esp_log_level_set("*", ESP_LOG_INFO);

    M5.begin();
    current_shooting_mode = Shooting_remote;
    // current_shooting_mode = Shooting_timelapse;
    current_page = Shooting;

    int do_pair = M5.BtnA.isPressed() ? 1 : 0;

    M5.Axp.ScreenBreath(9);
    M5_display.set_init_screen(do_pair);

    // M5.Beep.setVolume(1);
    M5.Beep.begin();

    timelapse.set_callbacks(&trigger);
    remote.set_callbacks(&beep, &trigger, &remote_finished);

    canon_ble.init();

    delay(500);

    // Pairing
    if (do_pair == 1)
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

    M5_display.set_init_screen(2);

    do
    {
        Serial.println("Waiting for camera...");
    } while (!canon_ble.forceCameraConnection());

    M5_display.set_address(canon_ble.getPairedAddressString());
    // M5_display.set_main_menu_screen(timelapse.get_interval(), "Ready for single shot");
    set_current_mode_display();
}