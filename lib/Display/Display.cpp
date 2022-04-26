#include "Display.h"
#include <Arduino.h>
#include "M5StickCPlus.h"
#include "icon.c"
#include <string>

Display::Display(M5Display* tft, String name_remote):
    buffer(tft),
    name(name_remote)
{
    // buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());
    buffer.setTextColor((negatif ? TFT_WHITE : TFT_BLACK));
    buffer.setCursor(0, 0);
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
    last_rotation = 1;
    M5.Lcd.setRotation(1);

    buffer.deleteSprite();
    buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));
    
    buffer.setTextSize(2);
    buffer.setFreeFont(font_name);
    buffer.setTextDatum(0);
    buffer.drawString(name, 120 - (buffer.textWidth(name)/2.0), 20 +  font_name->glyph->height); 
    
    // buffer.setTextSize(1);
    // buffer.setFreeFont(font_text);
    // buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95); 
    if(state == 0){ // pairing
        buffer.setTextSize(1);
        buffer.setFreeFont(font_text);
        buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95); 
    }

    if(state == 1){ // waiting camera
        buffer.setTextSize(1);
        buffer.setFreeFont(font_text);
        buffer.drawString("Waiting for camera", 120 - (buffer.textWidth("Waiting for camera") / 2.0), 95); 
    }

    if(state == 2){ // sleep mode
        buffer.setTextSize(1);
        buffer.setFreeFont(font_text);
        buffer.drawString("Press a button...", 120 - (buffer.textWidth("Press a button...") / 2.0), 95); 
    }

    //display_buffer.setTextSize(2);
    //display_buffer.setCursor(10, 70);
    //display_buffer.printf("Waiting for connection");
    buffer.pushSprite(0,0);
}

void Display::set_settings_mode_screen(int mode) //0 for remote, 1 for timelapses
{
    if(last_rotation != 0) // have to rotate the display
    {
        M5.Lcd.setRotation(0);

        buffer.deleteSprite();
        buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 0;
        Serial.println("Rotated display");
    }

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.setTextDatum(0);

    buffer.drawString("Remote", 15, 50);
    buffer.drawString("Timelapse", 15, 100);

    buffer.fillTriangle(0, (mode == 0) ? 50 : 100, 
    8, (mode == 0) ? 58 : 108, 
    0, (mode == 0) ? 66 : 116,
    (negatif ? TFT_WHITE : TFT_BLACK));


    buffer.setTextDatum(1);
    buffer.drawString("mode", M5.Lcd.width() / 2, M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.pushSprite(0,0);
}

void Display::set_timelapse_menu_screen(int delay, String status)
{
    if(last_rotation != 1)
    {
        M5.Lcd.setRotation(1);

        buffer.deleteSprite();
        buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 1;
        Serial.println("Rotated display");
    }

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.setTextDatum(0);
    buffer.drawString(address, 120 - (buffer.textWidth(address)/2.0), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));
    buffer.drawString(status, 120 - (buffer.textWidth(status)/2.0), 112);
    buffer.drawLine(0, 107, M5.Lcd.width(), 107, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Interval (secs):", 30, 35);
    buffer.setFreeFont(font_titles_24);
    buffer.drawFloat(float(delay)/1000.0, 1, 30, 60);

    buffer.pushSprite(0,0);
}

void Display::set_remote_menu_screen(int delay, int shots, int interval, String status, int setting_mode) // setting: -1 -> no setting, 0 -> Delay, 1 -> Shots, 2 -> Interval
{
    if(last_rotation != 0)
    {
        M5.Lcd.setRotation(0);

        buffer.deleteSprite();
        buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        last_rotation = 0;
        Serial.println("Rotated display");
    }

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK)); // top line
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK)); // bottom line

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.setTextDatum(1); // start from top center

    buffer.drawString(status, M5.Lcd.width() / 2, M5.Lcd.height() - 23);

    buffer.setTextDatum(0);
    if(setting_mode == -1 || setting_mode == 0)
    {
        buffer.drawString("Delay", 5, 35);
    }

    if(setting_mode == -1 || setting_mode == 1)
    {
        buffer.drawString("Shots", 5, 98);
    }

    if(setting_mode == -1 || setting_mode == 2)
    {
        buffer.drawString("Interval", 5, 155);
    }

    buffer.setFreeFont(font_titles_18);
    buffer.setTextSize(1);
    buffer.setTextDatum(1);
    if(setting_mode == -1 || setting_mode == 0)
    {
        buffer.drawNumber(delay, M5.Lcd.width() / 2, 60);
    }

    if(setting_mode == -1 || setting_mode == 1)
    {
        buffer.drawNumber(shots, M5.Lcd.width() / 2, 123);
    }

    if(setting_mode == -1 || setting_mode == 2)
    {
        if(shots == 1)
        {
            buffer.drawString("-", M5.Lcd.width() / 2, 180);
        } 
        else 
        {
            buffer.drawFloat(float(interval) / 1000.0, 1, M5.Lcd.width() / 2, 180);
        }
    }

    drawBatteryText();

    drawBatteryIcon();

    buffer.pushSprite(0,0);
}

void Display::drawBatteryText()
{
    String batt_value = "--%";
    if(m_battery_perc != -1)
    {
        batt_value = String(m_battery_perc) + "%";
    }

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(1);
    buffer.setTextDatum(2); // start from top right

    buffer.drawString(batt_value, M5.Lcd.width() - 5, 7);
}

void Display::drawBatteryIcon()
{
    if(m_charging){
        int32_t x = 74;
        int32_t y = 3;

        buffer.fillTriangle(
        x + 7, y + 0,
        x + 0, y + 11,
        x + 5, y + 11, 
        (negatif ? TFT_WHITE : TFT_BLACK));

        buffer.fillTriangle(
        x + 5, y + 9, 
        x + 10, y + 9,
        x + 4, y + 20,
        (negatif ? TFT_WHITE : TFT_BLACK));
        }
}