#include "Display.h"

/* --------------------------------- Constructor -----------------------------------*/

Display::Display(M5Display *t_pLcd, String t_nameRemote) : m_displayBuffer(t_pLcd),
                                                           m_name(t_nameRemote)
{
    m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());
    m_displayBuffer.setTextColor((m_negative ? TFT_WHITE : TFT_BLACK));
    m_displayBuffer.setCursor(0, 0);
}

/* --------------------------------- Public methods ---------------------------------*/

void Display::setInitScreen(int t_state)
{
    log_i("state %d", t_state);
    m_lastRotation = 1;
    M5.Lcd.setRotation(1);

    m_displayBuffer.deleteSprite();
    m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    setTextAttributes(m_fontName, 2, 0);
    m_displayBuffer.drawString(m_name, 120 - (m_displayBuffer.textWidth(m_name) / 2.0),
                               20 + m_fontName->glyph->height);

    // buffer.setTextSize(1);
    // buffer.setFreeFont(m_fontText);
    // buffer.drawString("Pairing in progress", 120 - (buffer.textWidth("Pairing in progress") / 2.0), 95);
    if (t_state == 0) // pairing
    {
        setTextAttributes(m_fontText, 1, 0);
        m_displayBuffer.drawString("Pairing in progress",
                                   120 - (m_displayBuffer.textWidth("Pairing in progress") / 2.0),
                                   95);
    }

    if (t_state == 1) // waiting camera
    {
        setTextAttributes(m_fontText, 1, 0);
        m_displayBuffer.drawString("Waiting for camera",
                                   120 - (m_displayBuffer.textWidth("Waiting for camera") / 2.0),
                                   95);
    }

    if (t_state == 2) // sleep mode
    {
        setTextAttributes(m_fontText, 1, 0);
        m_displayBuffer.drawString("Press a button...",
                                   120 - (m_displayBuffer.textWidth("Press a button...") / 2.0),
                                   95);
    }

    // m_displayBuffer.setTextSize(2);
    // m_displayBuffer.setCursor(10, 70);
    // m_displayBuffer.printf("Waiting for connection");
    m_displayBuffer.pushSprite(0, 0);
}

void Display::setSettingsModeScreen(MainPrograms t_mode)
{
    if (m_lastRotation != 0) // have to rotate the display
    {
        M5.Lcd.setRotation(0);

        m_displayBuffer.deleteSprite();
        m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        m_lastRotation = 0;
        log_v("Rotated display");
    }

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    drawHeader(true);
    drawFooter(true, "mode");

    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Remote", 15, 60);
    m_displayBuffer.drawString("Timelapse", 15, 110);
    m_displayBuffer.drawString("Preferences", 15, 160);

    switch (t_mode)
    {
    case MainPrograms::REMOTE:
        m_displayBuffer.fillTriangle(0, 60,
                                     8, 68,
                                     0, 76,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;

    case MainPrograms::TIMELAPSE:
        m_displayBuffer.fillTriangle(0, 110,
                                     8, 118,
                                     0, 126,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;

    case MainPrograms::PREFERENCES:
        m_displayBuffer.fillTriangle(0, 160,
                                     8, 168,
                                     0, 176,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;
    }

    m_displayBuffer.pushSprite(0, 0);
}

void Display::setTimelapseMenuScreen(int t_delay, String t_status)
{
    log_i("status: %s", t_status.c_str());

    if (m_lastRotation != 1)
    {
        M5.Lcd.setRotation(1);

        m_displayBuffer.deleteSprite();
        m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        m_lastRotation = 1;
        log_v("Rotated display");
    }

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    drawHeader(false);
    drawFooter(false, t_status);

    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Interval (secs):", 30, 35);
    m_displayBuffer.setFreeFont(m_fontTitles24);
    m_displayBuffer.drawFloat(static_cast<float>(t_delay) / 1000.0, 1, 30, 60);

    m_displayBuffer.pushSprite(0, 0);
}

void Display::setRemoteMenuScreen(int t_delay, int t_shots, unsigned long t_interval, String t_status)
{
    log_i("remote screen");

    if (m_lastRotation != 0)
    {
        M5.Lcd.setRotation(0);

        m_displayBuffer.deleteSprite();
        m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        m_lastRotation = 0;
        log_v("Rotated display");
    }

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    drawHeader(true);
    drawFooter(true, t_status);

    drawDelay(t_delay);
    drawShots(t_shots);
    drawInterval(t_shots, t_interval);

    m_displayBuffer.pushSprite(0, 0);
}

void Display::setRemoteMenuSettings(int t_delay, int t_shots, unsigned long t_interval, RemoteSettingModes t_settingMode)
{
    log_i("setting: %d", t_settingMode);

    if (m_lastRotation != 0)
    {
        M5.Lcd.setRotation(0);

        m_displayBuffer.deleteSprite();
        m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        m_lastRotation = 0;
        log_v("Rotated display");
    }

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    drawHeader(true);
    drawFooter(true, "Settings");

    switch (t_settingMode)
    {
    case RemoteSettingModes::DELAY:
        drawDelay(t_delay);
        break;

    case RemoteSettingModes::SHOTS:
        drawShots(t_shots);
        break;

    case RemoteSettingModes::INTERVAL:
        drawInterval(t_shots, t_interval);
        break;
    }

    m_displayBuffer.pushSprite(0, 0);
}

void Display::setPreferencesScreen(PreferenceModes t_mode, bool t_led, bool t_buzzer)
{
    log_i("preferences screen");

    if (m_lastRotation != 0)
    {
        M5.Lcd.setRotation(0);

        m_displayBuffer.deleteSprite();
        m_displayBuffer.createSprite(M5.Lcd.width(), M5.Lcd.height());

        m_lastRotation = 0;
        log_v("Rotated display");
    }

    m_displayBuffer.fillRect(0, 0, M5.Lcd.width(), M5.Lcd.height(),
                             (m_negative ? TFT_BLACK : TFT_WHITE));

    drawHeader(true);

    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Display", 15, 60);
    m_displayBuffer.drawString("LED", 15, 110);
    m_displayBuffer.drawString("Buzzer", 15, 160);

    m_displayBuffer.drawRect(M5.Lcd.width() - 25, 64, 14, 15,
                             (m_negative ? TFT_WHITE : TFT_BLACK));
    m_displayBuffer.drawRect(M5.Lcd.width() - 25, 113, 14, 15,
                             (m_negative ? TFT_WHITE : TFT_BLACK));
    m_displayBuffer.drawRect(M5.Lcd.width() - 25, 163, 14, 15,
                             (m_negative ? TFT_WHITE : TFT_BLACK));

    if (m_negative)
    {
        m_displayBuffer.fillRect(M5.Lcd.width() - 25, 64, 14, 15,
                                 (m_negative ? TFT_WHITE : TFT_BLACK));
    }

    if (t_led)
    {
        m_displayBuffer.fillRect(M5.Lcd.width() - 25, 113, 14, 15,
                                 (m_negative ? TFT_WHITE : TFT_BLACK));
    }

    if (t_buzzer)
    {
        m_displayBuffer.fillRect(M5.Lcd.width() - 25, 163, 14, 15,
                                 (m_negative ? TFT_WHITE : TFT_BLACK));
    }

    switch (t_mode)
    {
    case PreferenceModes::DISPLAY_NEG:
        m_displayBuffer.fillTriangle(0, 60,
                                     8, 68,
                                     0, 76,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;

    case PreferenceModes::LED:
        m_displayBuffer.fillTriangle(0, 110,
                                     8, 118,
                                     0, 126,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;

    case PreferenceModes::BUZZER:
        m_displayBuffer.fillTriangle(0, 160,
                                     8, 168,
                                     0, 176,
                                     (m_negative ? TFT_WHITE : TFT_BLACK));
        break;

    default:
        break;
    }

    m_displayBuffer.pushSprite(0, 0);
}

bool Display::toggleDisplay()
{
    m_negative = !m_negative;
    return !m_negative;
}

void Display::setAddress(String t_extAddress)
{
    m_address = t_extAddress;
    m_address.toUpperCase();
}

void Display::setBatteryInfos(int8_t t_batteryPerc, bool t_charging)
{
    m_batteryPerc = t_batteryPerc;
    m_charging = t_charging;
}

/* --------------------------------- Private methods ---------------------------------*/

void Display::drawHeader(bool t_vertical)
{

    if (t_vertical)
    {
        m_displayBuffer.drawLine(0, 28, M5.Lcd.width(), 28,
                                 (m_negative ? TFT_WHITE : TFT_BLACK)); // top line

        drawBatteryText();

        drawBatteryIcon();
    }
    else
    {
        m_displayBuffer.drawLine(0, 28, M5.Lcd.width(), 28, (m_negative ? TFT_WHITE : TFT_BLACK));

        setTextAttributes(m_fontText, 1, 0);
        m_displayBuffer.drawString(m_address, 120 - (m_displayBuffer.textWidth(m_address) / 2.0), 5);
    }
}

void Display::drawFooter(bool t_vertical, String t_status)
{
    if (t_vertical)
    {
        m_displayBuffer.drawLine(0, M5.Lcd.height() - 28, M5.Lcd.width(), M5.Lcd.height() - 28,
                                 (m_negative ? TFT_WHITE : TFT_BLACK)); // bottom line

        setTextAttributes(m_fontText, 1, 1);
        m_displayBuffer.drawString(t_status, M5.Lcd.width() / 2, M5.Lcd.height() - 23);
    }
    else
    {
        m_displayBuffer.drawLine(0, 107, M5.Lcd.width(), 107, (m_negative ? TFT_WHITE : TFT_BLACK));

        setTextAttributes(m_fontText, 1, 0);
        m_displayBuffer.drawString(t_status, 120 - (m_displayBuffer.textWidth(t_status) / 2.0), 112);
    }
}

void Display::drawDelay(int t_delay)
{
    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Delay", 5, 35);

    setTextAttributes(m_fontTitles18, 1, 1);

    m_displayBuffer.drawNumber(t_delay, M5.Lcd.width() / 2, 60);
}

void Display::drawShots(int t_shots)
{
    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Shots", 5, 98);

    setTextAttributes(m_fontTitles18, 1, 1);

    m_displayBuffer.drawNumber(t_shots, M5.Lcd.width() / 2, 123);
}

void Display::drawInterval(int t_shots, unsigned long t_interval)
{
    setTextAttributes(m_fontText, 1, 0);

    m_displayBuffer.drawString("Interval", 5, 155);

    setTextAttributes(m_fontTitles18, 1, 1);

    if (t_shots == 1)
    {
        m_displayBuffer.drawString("-", M5.Lcd.width() / 2, 180);
    }
    else
    {
        m_displayBuffer.drawFloat(static_cast<float>(t_interval) / 1000.0, 1,
                                  M5.Lcd.width() / 2, 180);
    }
}

void Display::setTextAttributes(const GFXfont *t_pfont, uint8_t t_size, uint8_t t_datum)
{
    m_displayBuffer.setFreeFont(t_pfont);
    m_displayBuffer.setTextColor((m_negative ? TFT_WHITE : TFT_BLACK));
    m_displayBuffer.setTextSize(t_size);
    m_displayBuffer.setTextDatum(t_datum);
}

void Display::drawBatteryText()
{
    String batt_value = "--%";
    if (m_batteryPerc != -1)
    {
        batt_value = String(m_batteryPerc) + "%";
    }

    m_displayBuffer.setFreeFont(m_fontTextSmall);
    m_displayBuffer.setTextColor((m_negative ? TFT_WHITE : TFT_BLACK));
    m_displayBuffer.setTextSize(1);
    m_displayBuffer.setTextDatum(2); // start from top right

    m_displayBuffer.drawString(batt_value, M5.Lcd.width() - 5, 7);
}

void Display::drawBatteryIcon()
{
    if (m_charging)
    {
        int32_t x = 74;
        int32_t y = 3;

        m_displayBuffer.fillTriangle(
            x + 7, y + 0,
            x + 0, y + 11,
            x + 5, y + 11,
            (m_negative ? TFT_WHITE : TFT_BLACK));

        m_displayBuffer.fillTriangle(
            x + 5, y + 9,
            x + 10, y + 9,
            x + 4, y + 20,
            (m_negative ? TFT_WHITE : TFT_BLACK));
    }
}
