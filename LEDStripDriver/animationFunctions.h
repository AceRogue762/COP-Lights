#ifndef ANIMATIONFUNCTIONS_H
#define ANIMATIONFUNCTIONS_H

/**
 * Handler functions for animations. This is where the actual
 * code driving the LED strip goes. Each function must conform
 * to the FreeRTOS task structure by accepting pvParameters and
 * using vTaskDelay() instead of delay().
 */

#include "Arduino.h"

void copLightsFlashing(void * pvParameters) {
  (void) pvParameters;
}

void rainyDay(void * pvParameters) {
  (void) pvParameters;
}


#endif
