#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "M5StickCPlus.h"
#include <Arduino.h>

#include "Main_Programs.h"
#include "Preference_Modes.h"
#include "Remote_Setting_Modes.h"

class Display
{
public:
    Display(M5Display *t_pLcd, String t_nameRemote);

    void setInitScreen(int t_state);

    void setSettingsModeScreen(MainPrograms t_mode);

    void setTimelapseMenuScreen(int t_delay, String t_status);

    void setRemoteMenuScreen(int t_delay,
                             int t_shots,
                             unsigned long t_interval,
                             String t_status);

    void setRemoteMenuSettings(int t_delay,
                               int t_shots,
                               unsigned long t_interval,
                               RemoteSettingModes t_setting_mode);

    void setPreferencesScreen(PreferenceModes t_mode, bool t_led, bool t_buzzer);

    void drawHeader(bool t_vertical);
    void drawFooter(bool t_vertical, String t_status);

    void drawDelay(int delay);
    void drawShots(int shots);
    void drawInterval(int shots, unsigned long interval);

    bool toggleDisplay();
    void setAddress(String t_extAddress);
    void setBatteryInfos(int8_t t_batteryPerc, bool t_charging);

private:
    TFT_eSprite m_displayBuffer;
    String m_name;
    String m_address;
    int8_t m_batteryPerc = -1;
    bool m_charging = false;
    bool m_negative = true; // White text on black bg if true
    uint8_t m_lastRotation = -1;

    const GFXfont *m_fontName = &Yellowtail_32;
    const GFXfont *m_fontTitles18 = &FreeSerif18pt7b;
    const GFXfont *m_fontTitles24 = &FreeSerif24pt7b;
    const GFXfont *m_fontText = &FreeSerif12pt7b;
    const GFXfont *m_fontTextSmall = &FreeSerif9pt7b;

    void setTextAttributes(const GFXfont *t_font, uint8_t t_size, uint8_t t_datum);
    void drawBatteryText();
    void drawBatteryIcon();
};

#endif // DISPLAY_H_
