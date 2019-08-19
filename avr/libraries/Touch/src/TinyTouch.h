#ifndef TINY_TOUCH_H
#define TINY_TOUCH_H

#include "QTouch/include/touch_api_ptc.h"

class TinyTouch{
  public:
    TinyTouch();
    ~TinyTouch();
    void init();
    void TouchHandle();
    uint16_t get_value(uint8_t sensor_node);
};


#endif