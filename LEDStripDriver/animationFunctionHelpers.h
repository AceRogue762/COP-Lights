/**
 * Helper functions to initialize the strip and simplify common
 * animation tasks.
 */

// Create RGB colors used by animations
RgbColor red(SATURATION, 0, 0);
RgbColor blue(0, 0, SATURATION);
RgbColor yellow(SATURATION, SATURATION, 0);
RgbColor black(0, 0, 0);
RgbColor white(SATURATION);

// Create the NeoPixelBus strip object and init
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);

/** 
 * Initialize the NeoPixel interface
 */
void initLEDs() {
  strip.Begin();
  strip.Show();
}

/**
 * Use this function to set all pixels to a color
 */
void setAllPixels(RgbColor color) {
  for (int count = 0; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, color); 
  }   
  
  strip.Show();
}
