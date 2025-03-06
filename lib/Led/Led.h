#ifndef LED_H_
#define LED_H_

#include <Arduino.h>

class Led
{
public:
    Led();

    void on();
    void on(unsigned long t_duration);
    void off();
    void setDuration(unsigned long t_duration);
    void setRepeat(bool t_repeat, unsigned long t_pause);
    void update();

private:
    unsigned long m_count;
    unsigned long m_ledDuration;
    bool m_ledOn;
    bool m_repeat;
    unsigned long m_pauseCount;
    unsigned long m_pauseDuration;
};

#endif // LED_H_
