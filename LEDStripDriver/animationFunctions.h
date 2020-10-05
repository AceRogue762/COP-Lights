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
 * 
 */

#include "Arduino.h"

#include "animationFunctionHelpers.h"
#include "config.h"

/**
 * Odd numbered pixels are colored snow white. Even pixels
 * are colored red and green, swapping places each loop to
 * achieve a moving effect.
 */
void christmasRGDance(void * pvParameters) {
  (void) pvParameters;

  int evenIndex = 0;
  bool swapRG = false;

  RgbColor dimWhite = RgbColor(20);

  // Startup animation
  for (int pixelIndex = START_LED; pixelIndex <= LED_COUNT; pixelIndex++) {
    if (pixelIndex % 2 == 0) {
      if (evenIndex % 2 == 0) {
        strip.SetPixelColor(pixelIndex, red);
      } else {
        strip.SetPixelColor(pixelIndex, green);
      }

      evenIndex++;
    } else {
      strip.SetPixelColor(pixelIndex, dimWhite);
    }
   
    strip.Show();
  
    // Delay 5 milleseconds between pixels
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }

  evenIndex = 0;
  
  // Main animation loop
  while (true) {
    for (int pixelIndex = START_LED; pixelIndex <= LED_COUNT; pixelIndex++) {
      if (pixelIndex % 2 == 0) {
        if (evenIndex % 2 == 0) {
          if (swapRG)
            strip.SetPixelColor(pixelIndex, green);
          else
            strip.SetPixelColor(pixelIndex, red);
        } else {
          if (swapRG)
            strip.SetPixelColor(pixelIndex, red);
          else
            strip.SetPixelColor(pixelIndex, green);
        }

        evenIndex++;
      } else {
        strip.SetPixelColor(pixelIndex, dimWhite);
      }
    }

    strip.Show();

    swapRG = !swapRG;
    evenIndex = 0;
    
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

/**
 * Red and blue alternate halves of strip, with white 
 * flashes in between transitions
 */
void copLightsAlternating(void * pvParameters) {
  (void) pvParameters;

  // Whether to set red or blue first
  bool swapped = true;
  
  // Index for middle pixel
  int median = (LED_COUNT + START_LED) / 2;

  // Startup animation
  // Turn first half red
  for (int count = START_LED; count <= median; count++) {
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
    for (int count = START_LED; count <= median; count++) {
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
  int pixelIndex = START_LED;

  // Whether to set red or blue first
  bool swapped = true;
  
  // Index for middle pixel
  int median = (LED_COUNT + START_LED) / 2;
  int lineSize = 36; 

  // Startup animation
  // Red Line Out
  for (int count = START_LED; count <= median; count++) {
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
      pixelIndex = START_LED;
    }

    // Calculate the end of each line, constrain to within bounds of the strip
    int rLineEnd = constrain(pixelIndex - lineSize / 2, START_LED, median - lineSize / 2);
    int bLineEnd = constrain(LED_COUNT - pixelIndex + lineSize / 2, START_LED, LED_COUNT);

    // Update pixels
    strip.SetPixelColor(pixelIndex, red);
    strip.SetPixelColor(LED_COUNT - pixelIndex, blue);
    strip.SetPixelColor(rLineEnd, black);
    strip.SetPixelColor(bLineEnd, black);
    
    strip.Show();
    pixelIndex++;
  }
}

/**
 * Fading orange lines on a black background
 */
void halloweenOrange(void * pvParameters) {
  (void) pvParameters;
  // Setup
  int LineSize = 12;
  RgbColor orange(74, 20, 0);
  bool swap = false;

  for (int count = START_LED; count <= LED_COUNT; count += LineSize * 2) {
    for (int LineCount = count; LineCount <= count + LineSize; LineCount++) {
      strip.SetPixelColor(LineCount, orange);
      vTaskDelay(5 / portTICK_PERIOD_MS);
      strip.Show();
    }
  }
  
  // Main animation loop
  while (true) {
    for (int brightness = 74; brightness >= 0; brightness-=4) {
      for (int count = START_LED; count <= LED_COUNT; count += LineSize * 2) {
        for (int LineCount = count; LineCount <= count + LineSize; LineCount++) {
          if (swap) {
            int SwappedBrightness = 74 - brightness;
            strip.SetPixelColor(LineCount, RgbColor(SwappedBrightness, SwappedBrightness/4, 0));
          } else {
            strip.SetPixelColor(LineCount, RgbColor(brightness, brightness/4, 0));
          }       
        }  
              
        for (int SecondLineCount = count + LineSize; SecondLineCount <= count + LineSize * 2; SecondLineCount++) {
          if (!swap) {
            int SwappedBrightness = 74 - brightness;
            strip.SetPixelColor(SecondLineCount, RgbColor(SwappedBrightness, SwappedBrightness/4, 0));
          } else {
            strip.SetPixelColor(SecondLineCount, RgbColor(brightness, brightness/4, 0));
          }    
        }      
      }

      strip.Show();
      vTaskDelay(70 / portTICK_PERIOD_MS);
    }
      
     swap = !swap;
  }
}

/**
 * Fast, fading red white and blue
 */
void copLightsMix(void * pvParameters) {
  (void) pvParameters;

  SetRandomSeed();


  // Main animation loop
  while (true) {
    if (animations.IsAnimating())
    {
        // The normal loop just needs these two to run the active animations
        animations.UpdateAnimations();
        strip.Show();
    }
    else
    {
        // No animation runnning, start some 
        FadeInFadeOutRinseRepeat(0.4f); // 0.0 = black, 0.25 is normal, 0.5 is bright
    }
  }
}

/**
 * Simulated thunderstorm with random rainfall and lightning
 */
void rainyDay(void * pvParameters) {
  (void) pvParameters;

  RgbColor thisWhite = RgbColor(5);
  

  int LightningFrequency = 20;
  int count = START_LED;

  // Startup animation
  for (int count = START_LED; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, thisWhite);
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
      
      setAllPixels(thisWhite); 
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
      
      setAllPixels(thisWhite); 
  
      count = START_LED;
    }
    
    int Pixel1Index = random(START_LED, LED_COUNT);
    int Pixel2Index = random(START_LED, LED_COUNT);
    int Pixel3Index = random(START_LED, LED_COUNT);
    
    for (int count = 40; count >= 15; count--) {
       strip.SetPixelColor(Pixel1Index, RgbColor(0, 0, count));
       strip.SetPixelColor(Pixel2Index, RgbColor(0, 0, count));
       strip.SetPixelColor(Pixel3Index, RgbColor(0, 0, count));
       
       strip.Show(); 
    }
  
    strip.SetPixelColor(Pixel1Index, thisWhite);
    strip.SetPixelColor(Pixel2Index, thisWhite);
    strip.SetPixelColor(Pixel3Index, thisWhite);
    
    strip.Show();
  
    count++;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

/**
 * The super mellow green and yellow animation
 */
 void melloYello(void * pvParameters) {
  (void) pvParameters;

  int count = START_LED;
  int loopdirection = 0;

  RgbColor thisGreen(0, 15, 0);

  // Startup animation
  for (int count = START_LED; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, thisGreen);
    delay(5);
    strip.Show();
  }

  while (true) {
    if (count >= LED_COUNT) 
      loopdirection = 1;
    else if (count <= START_LED && loopdirection == 1)
      loopdirection = 0;

    strip.SetPixelColor(count, black);

    if (loopdirection == 0) {
       int brightness = 25;
       int trailEnd = constrain(count - 25, START_LED, LED_COUNT);
       
       for(int i = count; i >= trailEnd; i--) {
         strip.SetPixelColor(i, RgbColor (brightness, brightness, 0));
         brightness--;
       } 
       
       strip.SetPixelColor(trailEnd, thisGreen);
       count++;
     } else {
       int brightness = 25;
       int trailEnd = constrain(count + 25, START_LED, LED_COUNT);
       
       for(int i = count; i <= trailEnd; i++) {
         strip.SetPixelColor(i, RgbColor (brightness, brightness, 0));
         brightness--;
       } 
       
       strip.SetPixelColor(trailEnd, thisGreen);
       count--;
     }
      
      strip.Show();
      delay(25);
    }
 }

 /**
  * The Yule Log Fire Animation
  */
 void yuleLog(void * pvParameters) {
   (void) pvParameters;

    // Setup
    float brightness; // percentage, applied to the whole strip
    int red, green;   // randomized red and green values
    RgbColor color;

    int lineSize = 10;
    int nextCrackle, crackleCounter;
    
    // Spark the flame
    for (int spark = 0; spark <= 2; spark++) {
      for (int count = START_LED; count <= LED_COUNT / 2; count+= 10) {
        strip.SetPixelColor(count, white);
        strip.SetPixelColor(LED_COUNT - count, white);
        strip.Show();
      }

      setAllPixels(black);
      vTaskDelay(750 / portTICK_PERIOD_MS);
    }

    // Fade in the initial fire color
    for (int brightness = 0; brightness <= 40; brightness++) {
      setAllPixels(RgbColor(brightness, brightness / 2, 0));
      strip.Show();
      vTaskDelay(75 / portTICK_PERIOD_MS);
    }

    // Main animation loop
    while (true) {
      // Set random brightness for entire strip
      brightness = random(60, 75) / 100.0;

      // Pick a random red/orange color for each pixel
      for (int count = START_LED; count <= LED_COUNT; count++) {
        red = random(40, 75);
        green = random(20, 30);      
  
        color = RgbColor(red * brightness, green * brightness, 0);
        
        strip.SetPixelColor(count, color);
      }

      // Crackle
      if (crackleCounter >= nextCrackle) {
        int crackleStart = random(START_LED, LED_COUNT - lineSize);

        // Show the crackle animation
        for (int count = crackleStart; count <= crackleStart + lineSize; count++) {
          int crackleRed = random(70, 95);
          int crackleYellow = random(25, 35);
          strip.SetPixelColor(count, RgbColor(crackleRed, crackleYellow, 0));
        }

        // Reset counter and randomize next crackle time
        crackleCounter = 0;
        nextCrackle = random(2, 10);
      }

      strip.Show();
      crackleCounter++;
  
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }  
  }

#endif
