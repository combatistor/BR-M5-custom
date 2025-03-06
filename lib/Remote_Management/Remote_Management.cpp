#include "Remote_Management.h"

/* --------------------------------- Constructor -----------------------------------*/

Remote::Remote(RemotePreferences *t_pPreferences,
               void (*t_pFuncBeep)(int), void (*t_pFuncTrigger)(), void (*t_pFuncFinished)())
    : m_pPreferences(t_pPreferences),
      m_pFuncBeep(t_pFuncBeep),
      m_pFuncTrigger(t_pFuncTrigger),
      m_pFuncFinished(t_pFuncFinished),
      m_startedCountdown(false),
      m_startedShots(false),
      m_delay(t_pPreferences->getDelay()),
      m_shots(t_pPreferences->getShots()),
      m_interval(t_pPreferences->getInterval())
{
}

/* --------------------------------- Public methods ---------------------------------*/

void Remote::update()
{
    if (m_startedCountdown)
    {
        if (m_delay == 0) // bypass countdown
        {
            startTrigger();
        }
        else // apply countdown
        {
            if (m_remainingDelay < 2)
            {
                if (millis() > m_125) // every 125ms
                {
                    m_125 = millis() + 125;

                    log_v("QUICK BEEP");
                    m_pFuncBeep(50);
                }
            }
            else
            {
                if (millis() > m_500) // every 500ms
                {
                    m_500 = millis() + 500;

                    log_v("BEEP");
                    m_pFuncBeep(200);
                }
            }

            if (millis() > m_1000) // every second
            {
                if (m_remainingDelay > 0)
                {
                    log_d("Remaining seconds: %d", m_remainingDelay);

                    m_1000 = millis() + 1000;

                    m_remainingDelay--;
                }
                else
                {
                    log_i("Start Shots logic");

                    startTrigger();
                }
            }
        }
    }

    if (m_startedShots)
    {
        if (m_shots == 1) // No need to apply any interval
        {
            log_v("TRIGGER!");
            m_pFuncTrigger();
            m_startedShots = false;

            m_pFuncFinished();
        }
        else
        {
            if (millis() > m_timeNextTrigger)
            {
                if (m_remainingShots > 0)
                {
                    log_d("Remaining shots: %d", m_remainingShots);

                    m_timeNextTrigger = millis() + m_interval;

                    m_remainingShots--;

                    log_v("TRIGGER!");
                    m_pFuncTrigger();
                }
                else
                {
                    log_i("Stop shots logic");

                    m_startedShots = false;

                    m_pFuncFinished();
                }
            }
        }
    }
}

void Remote::incDelay()
{
    m_delay += DELAY_INCREMENT;
    m_pPreferences->setDelay(m_delay);
}

void Remote::decDelay()
{
    if (m_delay - DELAY_INCREMENT < MIN_DELAY)
    {
        m_delay = MIN_DELAY;
    }
    else
    {
        m_delay -= DELAY_INCREMENT;
    }
    m_pPreferences->setDelay(m_delay);
}

void Remote::incShots()
{
    m_shots += SHOTS_INCREMENT;
    m_pPreferences->setShots(m_shots);
}

void Remote::decShots()
{
    if (m_shots - SHOTS_INCREMENT < MIN_SHOTS)
    {
        m_shots = MIN_SHOTS;
    }
    else
    {
        m_shots -= SHOTS_INCREMENT;
    }
    m_pPreferences->setShots(m_shots);
}

void Remote::incInterval()
{
    m_interval += INTERVAL_INCREMENT;
    m_pPreferences->setInterval(m_interval);
}

void Remote::decInterval()
{
    if (m_interval - INTERVAL_INCREMENT < MIN_INTERVAL)
    {
        m_interval = MIN_INTERVAL;
    }
    else
    {
        m_interval -= INTERVAL_INCREMENT;
    }
    m_pPreferences->setInterval(m_interval);
}

bool Remote::startOnOFF()
{
    if (m_startedCountdown || m_startedShots)
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

void Remote::startNow()
{
    m_pFuncTrigger();
    m_startedShots = true;

    m_pFuncFinished();
}

bool Remote::isStarted()
{
    return m_startedCountdown;
}

int Remote::getDelay()
{
    return m_delay;
}

int Remote::getShots()
{
    return m_shots;
}

int Remote::getInterval()
{
    return m_interval;
}

/* --------------------------------- Private methods ---------------------------------*/

void Remote::launch()
{
    m_remainingDelay = m_delay;
    m_remainingShots = m_shots;

    m_startedCountdown = true;
    m_startedShots = false;
}

void Remote::stop()
{
    m_startedCountdown = false;
    m_startedShots = false;

    m_remainingDelay = -1;
    m_remainingShots = -1;
}

void Remote::startTrigger()
{
    m_startedCountdown = false;

    log_i("START SHOTS");
    m_startedShots = true;
}