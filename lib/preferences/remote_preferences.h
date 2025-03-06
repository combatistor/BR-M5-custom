#ifndef REMOTE_PREFERENCES_H_
#define REMOTE_PREFERENCES_H_

#include <Arduino.h>
#include <Preferences.h>

#include "main_programs.h"

class RemotePreferences
{
public:
    static constexpr int DEFAULT_BRIGHTNESS = 50;
    static constexpr int DEFAULT_LOW_BRIGHTNESS = 15;

    static constexpr int DEFAULT_DELAY = 10;
    static constexpr int DEFAULT_SHOTS = 1;
    static constexpr unsigned long DEFAULT_INTERVAL = 1000;

    static constexpr unsigned long DEFAULT_TIMELAPSE_INTERVAL = 1000;

    RemotePreferences(Preferences *t_pPreferences);

    void setShootingMode(MainPrograms t_mode);
    MainPrograms getShootingMode();

    void setBrightness(int t_brightness);
    int getBrightness();

    void setDelay(int t_delay);
    int getDelay();

    void setShots(int t_shots);
    int getShots();

    void setInterval(unsigned long t_interval);
    unsigned long getInterval();

    void setTimelapseInterval(unsigned long t_interval);
    unsigned long getTimelapseInterval();

    void setDisplayNeg(bool t_neg);
    bool getDisplayNeg();

    void setDeviceLed(bool t_led);
    bool getDeviceLed();

    void setDeviceBuzzer(bool t_beep);
    bool getDeviceBuzzer();

private:
    const char *SHOOTING_MODE_KEY = "shoot_mode";

    const char *BRIGHTNESS_KEY = "brightness";

    const char *DELAY_KEY = "delay";
    const char *SHOTS_KEY = "shots";
    const char *INTERVAL_KEY = "interval";

    const char *TIMELAPSE_INTERVAL_KEY = "tl_interval";

    const char *DISPLAY_NEG_KEY = "disp_neg";
    const char *LED_KEY = "led";
    const char *BUZZER_KEY = "buzzer";

    Preferences *m_pPreferences;
};

#endif // REMOTE_PREFERENCES_H_
