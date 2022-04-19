#include "Remote_Management.h"

Remote::Remote()
{
    started_countdown = false;
    started_shots = false;
}

Remote::Remote(int delay, int shots, long interval)
{
    started_countdown = false;
    started_shots = false;

    this->Delay = delay;
    this->Shots = shots;
    this->Interval = interval;
}

void Remote::set_callbacks(void (*_func_beep)(int), void (*_func_trigger)(), void (*_func_finished)())
{
    this->func_beep = _func_beep;
    this->func_trigger = _func_trigger;
    this->func_finished = _func_finished;
}

void Remote::Remote_Update()
{
    if (started_countdown){
        if(Delay == 0) // bypass countdown
        {
            start_trigger();
        }
        else       // apply countdown
        {
            if(remaining_delay < 2)
            {
                if(millis() > t_125) // every 125ms
                {
                    t_125 = millis() + 125;

                    Serial.println("QUICK BEEP");
                    func_beep(50);   
                }
            }  
            else 
            {
                if(millis() > t_500) // every 500ms
                {
                    t_500 = millis() + 500; 

                    Serial.println("BEEP");
                    func_beep(200);
                }     

            }

            if(millis() > t_1000) // every second
            {
                if(remaining_delay > 0)
                {
                    Serial.println("Remaining seconds:");
                    Serial.println(remaining_delay);

                    t_1000 = millis() + 1000;

                    remaining_delay--;
                } else {
                    Serial.println("Start Shots logic");

                    start_trigger();
                }
            }
        }
    }

    if (started_shots)
    {
        if(Shots == 1) // No need to apply any interval
        {
            Serial.println("TRIGGER!");
            func_trigger();
            started_shots = false;

            func_finished();
        }
        else 
        {
            if(millis() > time_next_trigger)
            {
                if(remaining_shots > 0)
                {
                    Serial.println("Remaining shots:");
                    Serial.println(remaining_shots);

                    time_next_trigger = millis() + Interval;

                    remaining_shots--;

                    Serial.println("TRIGGER!");
                    func_trigger();
                } else {
                    Serial.println("Stop shots logic");

                    started_shots = false;

                    func_finished();
                }
            }
        }
    }
}

void Remote::start_trigger()
{
    started_countdown = false;
    
    Serial.println("START SHOTS");
    started_shots = true;
}

void Remote::Remote_incDelay()
{
    Delay += _delay_increment;
}

void Remote::Remote_decDelay()
{
    if (Delay - _delay_increment < _MIN_Delay)
    {
        Delay = _MIN_Delay;
    }
    else
    {
        Delay -= _delay_increment;
    }
}

void Remote::Remote_incShots()
{
    Shots += _shots_increment;
}

void Remote::Remote_decShots()
{
    if (Shots - _shots_increment < _MIN_Shots)
    {
        Shots = _MIN_Shots;
    }
    else
    {
        Shots -= _shots_increment;
    }
}

void Remote::Remote_incInterval()
{
    Interval += _interval_increment;
}

void Remote::Remote_decInterval()
{
    if (Interval - _interval_increment < _MIN_Interval)
    {
        Interval = _MIN_Interval;
    }
    else
    {
        Interval -= _interval_increment;
    }
}

void Remote::Launch()
{
    remaining_delay = Delay;
    remaining_shots = Shots;
   
    started_countdown = true;
    started_shots = false;
}

void Remote::Stop()
{
    started_countdown = false;
    started_shots = false;

    remaining_delay = -1;
    remaining_shots = -1;
}

bool Remote::Start_OnOFF()
{
    if (started_countdown || started_shots)
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

bool Remote::is_started()
{
    return started_countdown;
}

long Remote::get_delay()
{
    return Delay;
}

long Remote::get_shots()
{
    return Shots;
}

long Remote::get_interval()
{
    return Interval;
}