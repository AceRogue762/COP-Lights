#ifndef ANIMATIONFUNCTIONS_H
#define ANIMATIONFUNCTIONS_H

/**
 * Handler functions for animations. This is where the actual
 * code driving the LED strip goes. Each function must conform
 * to the FreeRTOS task structure by accepting pvParameters and
 * using vTaskDelay() instead of delay().
 * 
 * For example, to delay 100ms:
 * vTaskDelay(100 / portTICK_PERIOD_MS);
 */

#include "Arduino.h"

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelSegmentBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

#include "animationFunctionHelpers.h"
#include "config.h"

/* 
 * Boolean flag to tell running animation tasks that
 * they should stop (i.e. when a new animation is selected).
 * Animations should implement their task loop in this fashion:
 *
 * while (!shouldStopAnimation) {
 * ... animation code ...
 * }
 * vTaskDelete();
 */
bool shouldStopAnimation = false;

void copLightsAlternating(void * pvParameters) {
  (void) pvParameters;

  while (!shouldStopAnimation) {
    
  }

  vTaskDelete(NULL);
}

/**
 * Draws two moving lines, one red and one blue, which meet 
 * in the middle of the strip.
 */
void copLightsLineOut(void * pvParameters) {
  (void) pvParameters;

  shouldStopAnimation = false;

  // Current pixel being changed
  int pixelIndex = 0;

  // Whether to set red or blue first
  bool swapped = true;
  
  // Index for middle pixel
  int median = LED_COUNT / 2;
  int lineSize = 36; 

  // Startup animation
  // Red Line Out
  for (int count = 0; count <= median; count++) {
    strip.SetPixelColor(count, red);
    delay(5);
    strip.Show();
  }

  // Blue Line Out
  for (int count = median; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, blue);
    delay(5);
    strip.Show();
  }   

  // Turn off all pixels
  setAllPixels(black);


  // Main animation loop
  while (!shouldStopAnimation) {
    static RgbColor firstColor;
    static RgbColor secondColor;
  
    if(pixelIndex >= median){
      int NegCount = median;
      
      for (int count = median; count > median - lineSize / 2; count--) { 
        strip.SetPixelColor(count, white);
        strip.SetPixelColor(NegCount, white);
        strip.Show();
        NegCount++;
        delay(1);
      }
     
      setAllPixels(black);
      pixelIndex = 0;
    }
    
    strip.SetPixelColor(pixelIndex, red);
    strip.SetPixelColor(LED_COUNT - pixelIndex, blue);
    strip.SetPixelColor(LED_COUNT - lineSize / 2, black);
    strip.SetPixelColor(LED_COUNT - pixelIndex + lineSize / 2, black);
    
    strip.Show();
    pixelIndex++;
  }

  vTaskDelete(NULL);
}

void copLightsMix(void * pvParameters) {
  (void) pvParameters;

  shouldStopAnimation = false;

  while (!shouldStopAnimation) {
    
  }

  vTaskDelete(NULL);
}

void rainyDay(void * pvParameters) {
  (void) pvParameters;

  shouldStopAnimation = false;

  while (!shouldStopAnimation) {
    
  }

  vTaskDelete(NULL);
}


#endif
