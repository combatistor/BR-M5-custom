#include "Remote_Preferences.h"

/* --------------------------------- Constructor -----------------------------------*/

RemotePreferences::RemotePreferences(Preferences *t_pPreferences) : m_pPreferences(t_pPreferences)
{
}

/* --------------------------------- Public methods ---------------------------------*/

void RemotePreferences::setShootingMode(MainPrograms t_mode)
{
    m_pPreferences->putInt(SHOOTING_MODE_KEY, toUint8(t_mode));
}

MainPrograms RemotePreferences::getShootingMode()
{
    return (MainPrograms)m_pPreferences->getInt(SHOOTING_MODE_KEY, 0);
}

void RemotePreferences::setBrightness(int t_brightness)
{
    m_pPreferences->putInt(BRIGHTNESS_KEY, t_brightness);
}

int RemotePreferences::getBrightness()
{
    return m_pPreferences->getInt(BRIGHTNESS_KEY, RemotePreferences::DEFAULT_BRIGHTNESS);
}

void RemotePreferences::setDelay(int t_delay)
{
    m_pPreferences->putInt(DELAY_KEY, t_delay);
}

int RemotePreferences::getDelay()
{
    return m_pPreferences->getInt(DELAY_KEY, RemotePreferences::DEFAULT_DELAY);
}

void RemotePreferences::setShots(int t_shots)
{
    m_pPreferences->putInt(SHOTS_KEY, t_shots);
}

int RemotePreferences::getShots()
{
    return m_pPreferences->getInt(SHOTS_KEY, RemotePreferences::DEFAULT_SHOTS);
}

void RemotePreferences::setInterval(unsigned long t_interval)
{
    m_pPreferences->putULong(INTERVAL_KEY, t_interval);
}

unsigned long RemotePreferences::getInterval()
{
    return m_pPreferences->getULong(INTERVAL_KEY, RemotePreferences::DEFAULT_INTERVAL);
}

void RemotePreferences::setTimelapseInterval(unsigned long t_interval)
{
    m_pPreferences->putULong(TIMELAPSE_INTERVAL_KEY, t_interval);
}

unsigned long RemotePreferences::getTimelapseInterval()
{
    return m_pPreferences->getULong(TIMELAPSE_INTERVAL_KEY, RemotePreferences::DEFAULT_TIMELAPSE_INTERVAL);
}

void RemotePreferences::setDisplayNeg(bool t_neg)
{
    m_pPreferences->putBool(DISPLAY_NEG_KEY, t_neg);
}

bool RemotePreferences::getDisplayNeg()
{
    return m_pPreferences->getBool(DISPLAY_NEG_KEY, true);
}

void RemotePreferences::setDeviceLed(bool t_led)
{
    m_pPreferences->putBool(LED_KEY, t_led);
}

bool RemotePreferences::getDeviceLed()
{
    return m_pPreferences->getBool(LED_KEY, true);
}

void RemotePreferences::setDeviceBuzzer(bool t_beep)
{
    m_pPreferences->putBool(BUZZER_KEY, t_beep);
}

bool RemotePreferences::getDeviceBuzzer()
{
    return m_pPreferences->getBool(BUZZER_KEY, true);
}