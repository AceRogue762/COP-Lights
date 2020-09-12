#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include <NeoPixelSegmentBus.h>

// NeoPixel COPbrAlt
// This script alternatingly lights red and blue on each half

#include <NeoPixelBrightnessBus.h>

const uint16_t PixelCount = 55;
const uint16_t PixelPin = 4;
int PixelIndex = 0;


// Whether to set red or blue first
bool swapped = true;

// Index for middle pixel
int median = PixelCount / 2; //+ 20;
int LineSize = 2; 

#define colorSaturation 255

RgbColor red(colorSaturation, 0, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor black(0, 0, 0);
RgbColor white(colorSaturation);

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    strip.Begin();
    strip.Show();

    Serial.println();
    Serial.println("Running...");

    // Red Line Out
    for (int count = 0; count <= median; count++) {
      strip.SetPixelColor(count, red);
      delay(5);
      strip.Show();
    }

    // Blue Line Out
    for (int count = median; count <= PixelCount; count++) {
      strip.SetPixelColor(count, blue);
      delay(5);
      strip.Show();
    }    
    TurnAllOff();
}
void TurnAllOff() {
  for (int count = 0; count <= PixelCount; count++) {
    strip.SetPixelColor(count, black); 
  }   
    strip.Show();
}
void loop() {
  static RgbColor firstColor;
  static RgbColor secondColor;

  if(PixelIndex >= median){
          int NegCount = median;
    for (int count = median; count > median - LineSize / 2; count--) { 
      strip.SetPixelColor(count, white);
      strip.SetPixelColor(NegCount, white);
       strip.Show();
       NegCount++;
    delay(1);
    }
   
    TurnAllOff();
    PixelIndex = 0;
  }
  
  strip.SetPixelColor(PixelIndex, red);
  strip.SetPixelColor(PixelCount - PixelIndex, blue);
  strip.SetPixelColor(PixelIndex - LineSize / 2, black);
  strip.SetPixelColor(PixelCount - PixelIndex + LineSize / 2, black);
  strip.Show();
  PixelIndex++;
  //delay(1);
}
