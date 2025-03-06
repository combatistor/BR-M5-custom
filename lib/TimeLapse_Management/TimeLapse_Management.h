#ifndef TIMELAPSE_MANAGEMENT_H_
#define TIMELAPSE_MANAGEMENT_H_

#include <Arduino.h>

#include "Remote_Preferences.h"

class TimeLapse
{

public:
    TimeLapse(RemotePreferences *t_pPreferences, void (*t_pFuncTrigger)());

    void update();
    void incDelay();
    void decDelay();
    bool recordingOnOFF();

    unsigned long getInterval();
    bool isRecording();

private:
    static constexpr unsigned long MIN_INTERVAL = 600;    // ms
    static constexpr unsigned long DELAY_INCREMENT = 100; // ms

    RemotePreferences *m_pPreferences;
    bool m_timeLapseOn;
    unsigned long m_interval = 0;        // ms
    unsigned long m_timeNextTrigger = 0; // ms

    void (*m_pFuncTrigger)();

    void launch();
    void stop();
};

#endif // TIMELAPSE_MANAGEMENT_H_
