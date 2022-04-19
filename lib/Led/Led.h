#ifndef _LED_H_
#define _LED_H_

#include "Arduino.h"

class LED {
  public:
    LED(void);

    void off();
    void on();
    void on(uint16_t duration);
    void setDuration(uint16_t duration);
    void setRepeat(bool repeat, uint16_t pause);
    void update();

  private:
    uint32_t _count;
    uint16_t _led_duration;
    bool led_on;
    bool _repeat;
    uint32_t _pause_count;
    uint16_t _pause_duration;
};
#endif
