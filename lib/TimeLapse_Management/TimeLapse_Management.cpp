#include "TimeLapse_Management.h"

TimeLapse::TimeLapse()
{
    timeLapse_ON = false;
    interval = _MIN_Interval;
}

TimeLapse::TimeLapse(long default_interval)
{
    timeLapse_ON = false;
    if(default_interval < _MIN_Interval)
    {
        interval = _MIN_Interval;
    } 
    else 
    {
        interval = default_interval;
    }
    
}

void TimeLapse::set_callbacks(void (*_func_trigger)())
{
    this->func_trigger = _func_trigger;
}

void TimeLapse::TimeLapse_Update()
{
    if (timeLapse_ON && (millis() > _time_next_trigger)){
        _time_next_trigger = millis() + interval;
       
       func_trigger();
    }
}

void TimeLapse::TimeLapse_incDelay()
{
    interval += _delay_increment;
}

void TimeLapse::TimeLapse_decDelay()
{
    if (interval - _delay_increment >= _MIN_Interval)
    {
        interval -= _delay_increment;
    }
}

void TimeLapse::Launch()
{
    timeLapse_ON = true;
}

void TimeLapse::Stop()
{
    timeLapse_ON = false;
}

bool TimeLapse::Recording_OnOFF()
{
    if (timeLapse_ON)
    {
        Stop();
        return false;
    }
    else
    {
        Launch();
        return true;
    }
}

bool TimeLapse::is_recording()
{
    return timeLapse_ON;
}

long TimeLapse::get_interval()
{
    return interval;
}