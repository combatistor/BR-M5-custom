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

void Display::set_init_screen(int state)
{
    M5.Lcd.setRotation(1);

    buffer.deleteSprite();
    buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));
    
    buffer.setTextSize(2);
    buffer.setFreeFont(font_name);
    buffer.drawString(name, 120 - (buffer.textWidth(name)/2.0), 20 +  font_name->glyph->height); 
    
    // buffer.setTextSize(1);
    // buffer.setFreeFont(font_text);
    // buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95); 
    if(state == 1){ // pairing
        buffer.setTextSize(1);
        buffer.setFreeFont(font_text);
        buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95); 
    }

    if(state == 2){ // waiting camera
        buffer.setTextSize(1);
        buffer.setFreeFont(font_text);
        buffer.drawString("Waiting for camera", 120 - (buffer.textWidth("Waiting for camera") / 2.0), 95); 
    }

    //display_buffer.setTextSize(2);
    //display_buffer.setCursor(10, 70);
    //display_buffer.printf("Waiting for connection");
    buffer.pushSprite(0,0);
}

void Display::set_address(String address_str)
{
    address = address_str;
    address.toUpperCase();
}

void Display::set_timelapse_menu_screen(int delay, String status)
{
    M5.Lcd.setRotation(1);

    buffer.deleteSprite();
    buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);
    buffer.drawString(address, 120 - (buffer.textWidth(address)/2.0), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));
    buffer.drawString(status, 120 - (buffer.textWidth(status)/2.0), 112);
    buffer.drawLine(0, 107, M5.Lcd.width(), 107, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Interval (secs):", 30, 35);
    buffer.setFreeFont(font_titles_24);
    buffer.drawFloat(float(delay)/1000.0, 1, 30, 60);

    buffer.pushSprite(0,0);
}

void Display::set_remote_menu_screen(int delay, int shots, int interval, String status)
{
    M5.Lcd.setRotation(0);

    buffer.deleteSprite();
    buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(1);

    buffer.drawString(address, get_x_center_text(address), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);

    buffer.drawString(status, get_x_center_text(status), M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Delay", 5, 35);
    buffer.drawString("Shots", 5, 98);
    buffer.drawString("Interval", 5, 155);

    buffer.setFreeFont(font_titles_18);
    buffer.drawNumber(delay, get_x_center_text(String(delay)), 60);
    buffer.drawNumber(shots, get_x_center_text(String(shots)), 123);

    if(shots == 1)
    {
        buffer.drawString("-", get_x_center_text("-"), 180);
    } 
    else 
    {
        buffer.drawFloat(float(interval) / 1000.0, 1, get_x_center_text(String(float(interval)/1000.0, 1)), 180);
    }

    buffer.pushSprite(0,0);
}

void Display::set_settings_mode_screen(int mode) //0 for remote, 1 for timelapses
{
    M5.Lcd.setRotation(0);

    buffer.deleteSprite();
    buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);

    buffer.drawString("Remote", 15, 50);
    buffer.drawString("Timelapse", 15, 100);

    buffer.fillTriangle(0, (mode == 0) ? 50 : 100, 
    8, (mode == 0) ? 58 : 108, 
    0, (mode == 0) ? 66 : 116,
    (negatif ? TFT_WHITE : TFT_BLACK));


    buffer.drawString("mode", get_x_center_text("mode"), M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.pushSprite(0,0);
}

void Display::set_settings_remote_delay_screen(int delay)
{
    // buffer.deleteSprite();
    // buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(1);

    buffer.drawString(address, get_x_center_text(address), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);

    String status = "settings";
    buffer.drawString(status, get_x_center_text(status), M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Delay", 5, 35);

    buffer.setFreeFont(font_titles_18);
    buffer.drawNumber(delay, get_x_center_text(String(delay)), 60);

    buffer.pushSprite(0,0);
}

void Display::set_settings_remote_shots_screen(int shots)
{
    // buffer.deleteSprite();
    // buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(1);

    buffer.drawString(address, get_x_center_text(address), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);

    String status = "settings";
    buffer.drawString(status, get_x_center_text(status), M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Shots", 5, 98);

    buffer.setFreeFont(font_titles_18);
    buffer.drawNumber(shots, get_x_center_text(String(shots)), 123);

    buffer.pushSprite(0,0);
}

void Display::set_settings_remote_interval_screen(int shots, int interval)
{
    // buffer.deleteSprite();
    // buffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    buffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(), (negatif ? TFT_BLACK : TFT_WHITE));

    buffer.setFreeFont(font_text_small);
    buffer.setTextSize(1);

    buffer.drawString(address, get_x_center_text(address), 5);
    buffer.drawLine(0, 28, M5.Lcd.width(), 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.setFreeFont(font_text);
    buffer.setTextSize(1);

    String status = "settings";
    buffer.drawString(status, get_x_center_text(status), M5.Lcd.height() - 23);
    buffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28, (negatif ? TFT_WHITE : TFT_BLACK));

    buffer.drawString("Interval", 5, 155);

    if(shots == 1)
    {
        buffer.setFreeFont(font_titles_18);
        buffer.drawString("-", get_x_center_text("-"), 180);
    } 
    else 
    {
        buffer.setFreeFont(font_titles_18);
        buffer.drawFloat(float(interval)/1000.0, 1, get_x_center_text(String(float(interval)/1000.0, 1)), 180);
    }

    buffer.pushSprite(0,0);
}

int32_t Display::get_x_center_text(String text)
{
    return (M5.Lcd.width() / 2) - (buffer.textWidth(text) / 2.0);
}