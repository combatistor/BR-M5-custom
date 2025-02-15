#include "Led.h"

#include "M5StickCPlus.h"

LED::LED(void) {
  pinMode(M5_LED, OUTPUT);
  digitalWrite(M5_LED, HIGH);
}

void LED::on() {
  on(_led_duration);
}

void LED::on(uint16_t duration) {
  digitalWrite(M5_LED, LOW);
  log_v("LED ON");

  if(duration == 0) // infinite
  {
    _count = -1;
  } 
  else 
  {
    _count = millis() + duration;
  }
  
  led_on = true;
}

void LED::setDuration(uint16_t duration) { // 0 for infinite
  _led_duration = duration;
}

void LED::setRepeat(bool repeat, uint16_t pause) {
  _repeat = repeat;
  if(repeat)
  {
    _pause_duration = pause;
  } 
  else 
  {
    _pause_duration = 0;
  }
}

void LED::off() {
  digitalWrite(M5_LED, HIGH);
  log_v("LED OFF");

  if(_repeat){
    _pause_count = millis() + _pause_duration;
  }
}

void LED::update() {
  if(led_on) {
    if(_count != -1) // check infinite time
    {
      if(millis() > _count) 
      {
        led_on = false;
        off();
      }
    }
  } else {
    if(_repeat)
    {
      if(millis() > _pause_count) 
      {
        on();
      }
    }
  }
}