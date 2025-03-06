#include "TimeLapse_Management.h"

/* --------------------------------- Constructor -----------------------------------*/

TimeLapse::TimeLapse(RemotePreferences *t_pPreferences, void (*t_pFuncTrigger)())
    : m_pPreferences(t_pPreferences),
      m_pFuncTrigger(t_pFuncTrigger),
      m_timeLapseOn(false),
      m_interval(t_pPreferences->getTimelapseInterval())
{
}

/* --------------------------------- Public methods ---------------------------------*/

void TimeLapse::update()
{
    if (m_timeLapseOn && (millis() > m_timeNextTrigger))
    {
        m_timeNextTrigger = millis() + m_interval;

        m_pFuncTrigger();
    }
}

void TimeLapse::incDelay()
{
    m_interval += DELAY_INCREMENT;
    m_pPreferences->setTimelapseInterval(m_interval);
}

void TimeLapse::decDelay()
{
    if (m_interval - DELAY_INCREMENT >= MIN_INTERVAL)
    {
        m_interval -= DELAY_INCREMENT;
    }
    m_pPreferences->setTimelapseInterval(m_interval);
}

bool TimeLapse::recordingOnOFF()
{
    if (m_timeLapseOn)
    {
        stop();
        return false;
    }
    else
    {
        launch();
        return true;
    }
}

bool TimeLapse::isRecording()
{
    return m_timeLapseOn;
}

unsigned long TimeLapse::getInterval()
{
    return m_interval;
}

/* --------------------------------- Private methods ---------------------------------*/

void TimeLapse::launch()
{
    m_timeLapseOn = true;
}

void TimeLapse::stop()
{
    m_timeLapseOn = false;
}