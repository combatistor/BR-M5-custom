#include "Display.h"
#include <Arduino.h>
#include "M5StickCPlus.h"
#include "icon.c"
#include <string>

int Display::BRIGHTNESS = 50;
int Display::LOW_BRIGHTNESS = 15;

Display::Display(M5Display* lcd, String name_remote):
    display_buffer(lcd),
    name(name_remote)
{
    display_buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());
    display_buffer.setTextColor((negatif ? TFT_WHITE : TFT_BLACK));
    display_buffer.setCursor(0, 0);
}

void Display::setBatteryInfos(int8_t battery_perc, bool charging)
{
    m_battery_perc = battery_perc;
    m_charging = charging;
}

void Display::set_address(String address_str)
{
    address = address_str;
    address.toUpperCase();
}

void Display::set_init_screen(int state)
{
    log_i("state %d", state);
    last_rotation = 1;
    M5.Lcd.setRotation(1);

    display_buffer.deleteSprite();
    display_buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    display_buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));
    
    display_buffer.setTextSize(2);
    display_buffer.setFreeFont(font_name);
    display_buffer.setTextDatum(0);
    display_buffer.drawString(name, 120 - (display_buffer.textWidth(name)/2.0), 20 +  font_name->glyph->height); 
    
    // buffer.setTextSize(1);
    // buffer.setFreeFont(font_text);
    // buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95); 
    if(state == 0){ // pairing
        display_buffer.setTextSize(1);
        display_buffer.setFreeFont(font_text);
        display_buffer.drawString("Pairing in progress", 120 - (display_buffer.textWidth("Pairing in progress") / 2.0), 95); 
    }

    if(state == 1){ // waiting camera
        display_buffer.setTextSize(1);
        display_buffer.setFreeFont(font_text);
        display_buffer.drawString("Waiting for camera", 120 - (display_buffer.textWidth("Waiting for camera") / 2.0), 95); 
    }

    if(state == 2){ // sleep mode
        display_buffer.setTextSize(1);
        display_buffer.setFreeFont(font_text);
        display_buffer.drawString("Press a button...", 120 - (display_buffer.textWidth("Press a button...") / 2.0), 95); 
    }

    //display_buffer.setTextSize(2);
    //display_buffer.setCursor(10, 70);
    //display_buffer.printf("Waiting for connection");
    display_buffer.pushSprite(0,0);
}

void Display::set_settings_mode_screen(int mode) //0 for remote, 1 for timelapses
{
    if(last_rotation != 0) // have to rotate the display
    {
        M5.Lcd.setRotation(0);

        display_buffer.deleteSprite();
        display_buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 0;
        log_v("Rotated display");
    }

    display_buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    display_buffer.setFreeFont(font_text);
    display_buffer.setTextSize(1);
    display_buffer.setTextDatum(0);

    display_buffer.drawString("Remote", 15, 50);
    display_buffer.drawString("Timelapse", 15, 100);

    display_buffer.fillTriangle(0, (mode == 0) ? 50 : 100, 
    8, (mode == 0) ? 58 : 108, 
    0, (mode == 0) ? 66 : 116,
    (negatif ? TFT_WHITE : TFT_BLACK));


    display_buffer.setTextDatum(1);
    display_buffer.drawString("mode", M5.Lcd.width() / 2, M5.Lcd.height() - 23);
    display_buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    display_buffer.pushSprite(0,0);
}

void Display::set_timelapse_menu_screen(int delay, String status)
{
    log_i("set_timelapse_menu_screen, status: %s", status.c_str());

    if(last_rotation != 1)
    {
        M5.Lcd.setRotation(1);

        display_buffer.deleteSprite();
        display_buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 1;
        log_v("Rotated display");
    }

    display_buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    display_buffer.setFreeFont(font_text);
    display_buffer.setTextSize(1);
    display_buffer.setTextDatum(0);
    display_buffer.drawString(address, 120 - (display_buffer.textWidth(address)/2.0), 5);
    display_buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));
    display_buffer.drawString(status, 120 - (display_buffer.textWidth(status)/2.0), 112);
    display_buffer.drawLine(0, 107, M5.Lcd.width(), 107, (negatif ? TFT_WHITE : TFT_BLACK));

    display_buffer.drawString("Interval (secs):", 30, 35);
    display_buffer.setFreeFont(font_titles_24);
    display_buffer.drawFloat(float(delay)/1000.0, 1, 30, 60);

    display_buffer.pushSprite(0,0);
    display_buffer.endWrite();
}

void Display::set_remote_menu_screen(int delay, int shots, int interval, String status, int setting_mode) // setting: -1 -> no setting, 0 -> Delay, 1 -> Shots, 2 -> Interval
{
    log_i("setting: %d", setting_mode);

    if(last_rotation != 0)
    {
        M5.Lcd.setRotation(0);

        display_buffer.deleteSprite();
        display_buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 0;
        log_v("Rotated display");
    }

    display_buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    display_buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK)); // top line
    display_buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK)); // bottom line

    display_buffer.setFreeFont(font_text);
    display_buffer.setTextSize(1);
    display_buffer.setTextDatum(1); // start from top center

    display_buffer.drawString(status, M5.Lcd.width() / 2, M5.Lcd.height() - 23);

    display_buffer.setTextDatum(0);
    if(setting_mode == -1 || setting_mode == 0)
    {
        display_buffer.drawString("Delay", 5, 35);
    }

    if(setting_mode == -1 || setting_mode == 1)
    {
        display_buffer.drawString("Shots", 5, 98);
    }

    if(setting_mode == -1 || setting_mode == 2)
    {
        display_buffer.drawString("Interval", 5, 155);
    }

    display_buffer.setFreeFont(font_titles_18);
    display_buffer.setTextSize(1);
    display_buffer.setTextDatum(1);
    if(setting_mode == -1 || setting_mode == 0)
    {
        display_buffer.drawNumber(delay, M5.Lcd.width() / 2, 60);
    }

    if(setting_mode == -1 || setting_mode == 1)
    {
        display_buffer.drawNumber(shots, M5.Lcd.width() / 2, 123);
    }

    if(setting_mode == -1 || setting_mode == 2)
    {
        if(shots == 1)
        {
            display_buffer.drawString("-", M5.Lcd.width() / 2, 180);
        } 
        else 
        {
            display_buffer.drawFloat(float(interval) / 1000.0, 1, M5.Lcd.width() / 2, 180);
        }
    }

    drawBatteryText();

    drawBatteryIcon();

    display_buffer.pushSprite(0,0);
}

void Display::drawBatteryText()
{
    String batt_value = "--%";
    if(m_battery_perc != -1)
    {
        batt_value = String(m_battery_perc) + "%";
    }

    display_buffer.setFreeFont(font_text_small);
    display_buffer.setTextSize(1);
    display_buffer.setTextDatum(2); // start from top right

    display_buffer.drawString(batt_value, M5.Lcd.width() - 5, 7);
}

void Display::drawBatteryIcon()
{
    if(m_charging){
        int32_t x = 74;
        int32_t y = 3;

        display_buffer.fillTriangle(
        x + 7, y + 0,
        x + 0, y + 11,
        x + 5, y + 11, 
        (negatif ? TFT_WHITE : TFT_BLACK));

        display_buffer.fillTriangle(
        x + 5, y + 9, 
        x + 10, y + 9,
        x + 4, y + 20,
        (negatif ? TFT_WHITE : TFT_BLACK));
        }
}