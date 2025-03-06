#include "Led.h"

#include "M5StickCPlus.h"

/* --------------------------------- Constructor -----------------------------------*/

Led::Led()
{
    pinMode(M5_LED, OUTPUT);
    digitalWrite(M5_LED, HIGH);
}

/* --------------------------------- Public methods ---------------------------------*/

void Led::on()
{
    on(m_ledDuration);
}

void Led::on(unsigned long t_duration)
{
    digitalWrite(M5_LED, LOW);
    log_v("LED ON");

    if (t_duration == 0) // infinite
    {
        m_count = -1;
    }
    else
    {
        m_count = millis() + t_duration;
    }

    m_ledOn = true;
}

void Led::off()
{
    digitalWrite(M5_LED, HIGH);
    log_v("LED OFF");

    if (m_repeat)
    {
        m_pauseCount = millis() + m_pauseDuration;
    }
}

void Led::setDuration(unsigned long t_duration) // 0 for infinite
{
    m_ledDuration = t_duration;
}

void Led::setRepeat(bool t_repeat, unsigned long t_pause)
{
    m_repeat = t_repeat;
    if (m_repeat)
    {
        m_pauseDuration = t_pause;
    }
    else
    {
        m_pauseDuration = 0;
    }
}

void Led::update()
{
    if (m_ledOn)
    {
        if (m_count != -1) // check infinite time
        {
            if (millis() > m_count)
            {
                m_ledOn = false;
                off();
            }
        }
    }
    else
    {
        if (m_repeat)
        {
            if (millis() > m_pauseCount)
            {
                on();
            }
        }
    }
}