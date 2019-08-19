
#include "TinyTouch.h"


TinyTouch::TinyTouch(/* args */)
{
}

TinyTouch::~TinyTouch()
{
}

void TinyTouch::init()
{
  touch_init();
}

void TinyTouch::TouchHandle() {
  touch_process();
}

uint16_t TinyTouch::get_value(uint8_t sensor_node) {
  return get_sensor_node_signal(sensor_node);
}
