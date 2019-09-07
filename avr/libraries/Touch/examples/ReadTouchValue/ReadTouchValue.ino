
#include "TinyTouch.h"

#define TOUCH_TRIGGER_VALUE 800


TinyTouch touch;

uint8_t touchPins[2] = {13, 2}; //initialize touch pins

void setup() {
    touch.begin(touchPins, sizeof(touchPins));

    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    touch.touchHandle();

    if (touch.getValue(0) > TOUCH_TRIGGER_VALUE or touch.getValue(1) > TOUCH_TRIGGER_VALUE) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}