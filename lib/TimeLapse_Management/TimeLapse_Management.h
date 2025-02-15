#ifndef TimeLapse_Management_h
#define TimeLapse_Management_h

#include <Arduino.h>

class TimeLapse {
    public :
        static long DEFAULT_INTERVAL;
        //constructor
        //IN/ inter_min : default delay between shots (in ms)
        //IN/ trigger : function taking shot
        TimeLapse();
        TimeLapse(long default_interval);
        void set_callbacks(void (*_func_trigger)());

        //trigger each #interval ms
        void TimeLapse_Update();
        //increase #interval
        void TimeLapse_incDelay();
        //decrease #interval (> #_MIN_Interval)
        void TimeLapse_decDelay();
        bool Recording_OnOFF();
        long get_interval();
        bool is_recording();

    private :
        bool timeLapse_ON;
        long interval = 0; //ms

        void(*func_trigger)();

        long _time_next_trigger = 0; //ms
        long _MIN_Interval = 600; //ms
        const long _delay_increment = 100; //ms

        void Launch();
        void Stop();
};

#endif
