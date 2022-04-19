#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "M5StickCPlus.h"
#include <Arduino.h>

extern const unsigned char icon_ble[4608];

class Display
{
private:
    TFT_eSprite buffer;
    String name;
    String address;
    bool negatif = true; //White text on black bg if true

    const GFXfont* font_name = &Yellowtail_32;
    const GFXfont* font_titles_18 = &FreeSerif18pt7b;
    const GFXfont* font_titles_24 = &FreeSerif24pt7b;
    const GFXfont* font_text = &FreeSerif12pt7b;
    const GFXfont* font_text_small = &FreeSerif9pt7b;

    int32_t get_x_center_text(String text);
    
public :
    Display(M5Display* tft, String name_remote);
    void set_address(String ext_address);
    void set_init_screen(int state);
    void set_timelapse_menu_screen(int delay, String status);
    void set_remote_menu_screen(int delay, int shots, int interval, String status);

    void set_settings_mode_screen(int mode);
    void set_settings_remote_delay_screen(int delay);
    void set_settings_remote_shots_screen(int shots);
    void set_settings_remote_interval_screen(int shots, int interval);
};

#endif