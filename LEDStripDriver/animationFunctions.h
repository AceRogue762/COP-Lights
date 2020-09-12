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

#include "animationFunctionHelpers.h"
#include "config.h"

void copLightsAlternating(void * pvParameters) {
  (void) pvParameters;

  // Whether to set red or blue first
  bool swapped = true;
  
  // Index for middle pixel
  int median = LED_COUNT / 2;

  // Startup animation
  // Turn first half red
  for (int count = 0; count <= median; count++) {
    strip.SetPixelColor(count, red);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    strip.Show();
  }

  // Turn second half blue
  for (int count = median; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, blue);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    strip.Show();
  }

  // Main animation loop
  while (true) {
    static RgbColor firstColor;
    static RgbColor secondColor;
    
    if (swapped) {
      firstColor = red;
      secondColor = blue;
    } else {
      firstColor = blue;
      secondColor = red;
    }
  
    // Swap the boolean
    swapped = ! swapped;
    setAllPixels(white);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    
    // Set first half of strip
    for (int count = 0; count <= median; count++) {
      strip.SetPixelColor(count, firstColor);
    }
  
    // Set second half of strip
    for (int count = median; count <= LED_COUNT; count++) {
      strip.SetPixelColor(count, secondColor);
    }

    strip.Show();
    vTaskDelay(300 / portTICK_PERIOD_MS);
  }
}

/**
 * Draws two moving lines, one red and one blue, which meet 
 * in the middle of the strip.
 */
void copLightsLineOut(void * pvParameters) {
  (void) pvParameters;

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
    vTaskDelay(5 / portTICK_PERIOD_MS);
    strip.Show();
  }

  // Blue Line Out
  for (int count = median; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, blue);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    strip.Show();
  }   

  // Turn off all pixels
  setAllPixels(black);


  // Main animation loop
  while (true) {
    static RgbColor firstColor;
    static RgbColor secondColor;
  
    if(pixelIndex >= median){
      int NegCount = median;
      
      for (int count = median; count > median - lineSize / 2; count--) { 
        strip.SetPixelColor(count, white);
        strip.SetPixelColor(NegCount, white);
        strip.Show();
        NegCount++;
        vTaskDelay(1 / portTICK_PERIOD_MS);
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
}

void copLightsMix(void * pvParameters) {
  (void) pvParameters;

  SetRandomSeed();


  // Main animation loop
  while (true) {
    if (animations.IsAnimating())
    {
        // the normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        strip.Show();
    }
    else
    {
        // no animation runnning, start some 
        //
        FadeInFadeOutRinseRepeat(0.4f); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }
  }
}

/**
 * Simulated thunderstorm with random rainfall and lightning
 */
void rainyDay(void * pvParameters) {
  (void) pvParameters;

  int LightningFrequency = 20;
  int count = 0;

  // Startup animation
  for (int count = 0; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, white);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    strip.Show();
  }

  // Main animation loop
  while (true) {
    if (count % LightningFrequency == 0) {
      // Single lightning flash
      setAllPixels(black);
      
      setAllPixels(yellow);
    
      strip.Show();
      vTaskDelay(50 / portTICK_PERIOD_MS);
      
      setAllPixels(white); 
    }
  
    if (count % LightningFrequency * 2 == 0) {
      // Double lightning flash
      setAllPixels(black);
      
      setAllPixels(yellow);
    
      strip.Show();
      vTaskDelay(50 / portTICK_PERIOD_MS);
      
      setAllPixels(white); 
      strip.Show();
      vTaskDelay(10 / portTICK_PERIOD_MS);
  
      setAllPixels(black);
      
      setAllPixels(yellow);
    
      strip.Show();
      vTaskDelay(50 / portTICK_PERIOD_MS);
      
      setAllPixels(white); 
  
      count = 0;
    }
    
    int Pixel1Index = random(LED_COUNT);
    int Pixel2Index = random(LED_COUNT);
    int Pixel3Index = random(LED_COUNT);
    
    for (int count = SATURATION; count >= 15; count--) {
       strip.SetPixelColor(Pixel1Index, RgbColor(0, 0, count));
       strip.SetPixelColor(Pixel2Index, RgbColor(0, 0, count));
       strip.SetPixelColor(Pixel3Index, RgbColor(0, 0, count));
       
       strip.Show(); 
    }
  
    strip.SetPixelColor(Pixel1Index, white);
    strip.SetPixelColor(Pixel2Index, white);
    strip.SetPixelColor(Pixel3Index, white);
    
    strip.Show();
  
    count++;
  }
}


#endif
