#ifndef REMOTE_MANAGEMENT_H_
#define REMOTE_MANAGEMENT_H_

#include <Arduino.h>

#include "remote_preferences.h"

class Remote
{
public:
    Remote(RemotePreferences *t_pPreferences,
           void (*t_pFuncBeep)(int), void (*t_pFuncTrigger)(), void (*t_pFuncFinished)());

    void update();

    void incDelay();
    void decDelay();

    void incShots();
    void decShots();

    void incInterval();
    void decInterval();

    bool startOnOFF();
    void startNow();

    bool isStarted();

    int getDelay();
    int getShots();
    int getInterval();

private:
    static constexpr unsigned int MIN_DELAY = 0;  // s
    static constexpr unsigned int MIN_SHOTS = 1;
    static constexpr unsigned long MIN_INTERVAL = 600;  // ms

    static constexpr unsigned int DELAY_INCREMENT = 1;  // s
    static constexpr unsigned int SHOTS_INCREMENT = 1;
    static constexpr unsigned long INTERVAL_INCREMENT = 100;  // ms

    RemotePreferences *m_pPreferences;
    bool m_startedCountdown;
    bool m_startedShots;

    int m_remainingDelay = -1;
    int m_remainingShots = -1;

    int m_delay;  // ms
    int m_shots;
    unsigned long m_interval;  // ms

    void (*m_pFuncBeep)(int);
    void (*m_pFuncTrigger)();
    void (*m_pFuncFinished)();

    unsigned long m_125 = 0;
    unsigned long m_500 = 0;
    unsigned long m_1000 = 0;
    unsigned long m_timeNextTrigger = 0;

    void launch();
    void stop();

    void startTrigger();
};

#endif  // REMOTE_MANAGEMENT_H_
