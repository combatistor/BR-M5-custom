#ifndef Remote_Management_h
#define Remote_Management_h

#include <Arduino.h>

class Remote {
    public :
        //constructor
        Remote();
        Remote(int delay, int shots, long interval);

        void set_callbacks(void (*_func_beep)(int), void (*_func_trigger)(), void (*_func_finished)());
        
        void Remote_Update();
        void Remote_incDelay();
        void Remote_decDelay();
        void Remote_incShots();
        void Remote_decShots();
        void Remote_incInterval();
        void Remote_decInterval();
        long get_delay();
        long get_shots();
        long get_interval();
        bool is_started();
        bool Start_OnOFF();

    private :
        bool started_countdown;
        bool started_shots;

        long remaining_delay = -1;
        long remaining_shots = -1;

        long Delay = 10; //ms
        long Shots = 1; //ms
        long Interval = 1000; //ms

        void(*func_beep)(int);
        void(*func_trigger)();
        void (*func_finished)();

        long t_125 = 0;
        long t_500 = 0;
        long t_1000 = 0;
        long time_next_trigger = 0;

        long _MIN_Delay = 0; //s
        long _MIN_Shots = 1;
        long _MIN_Interval = 600; //ms
        const long _delay_increment = 1; //s
        const long _shots_increment = 1;
        const long _interval_increment = 100; //ms

        void Launch();
        void Stop();

        void start_trigger();
};

#endif
