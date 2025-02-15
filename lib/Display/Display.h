#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "M5StickCPlus.h"
#include <Arduino.h>

extern const unsigned char icon_ble[4608];

class Display
{
private:
    TFT_eSprite display_buffer;
    String name;
    String address;
    int8_t m_battery_perc = -1;
    bool m_charging = false;
    bool negatif = true; //White text on black bg if true
    uint8_t last_rotation = -1;

    const GFXfont* font_name = &Yellowtail_32;
    const GFXfont* font_titles_18 = &FreeSerif18pt7b;
    const GFXfont* font_titles_24 = &FreeSerif24pt7b;
    const GFXfont* font_text = &FreeSerif12pt7b;
    const GFXfont* font_text_small = &FreeSerif9pt7b;

    void drawBatteryText();
    void drawBatteryIcon();
    
public :
    Display(M5Display* lcd, String name_remote);

    static int BRIGHTNESS;
    static int LOW_BRIGHTNESS;

    void set_address(String ext_address);
    void setBatteryInfos(int8_t _battery_perc, bool charging);

    void set_init_screen(int state);
    void set_settings_mode_screen(int mode);

    void set_timelapse_menu_screen(int delay, String status);

    void set_remote_menu_screen(int delay, int shots, int interval, String status, int setting_mode);
};

#endif