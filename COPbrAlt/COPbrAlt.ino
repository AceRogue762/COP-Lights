#include <NeoPixelAnimator.h>
#include <NeoPixelBrightnessBus.h>
#include <NeoPixelBus.h>
#include <NeoPixelSegmentBus.h>

// NeoPixel COPbrAlt
// This script alternatingly lights red and blue on each half

#include <NeoPixelBrightnessBus.h>

const uint16_t PixelCount = 245;
const uint16_t PixelPin = 4;

// Whether to set red or blue first
bool swapped = true;

// Index for middle pixel
int median = PixelCount / 2 + 20;

#define colorSaturation 175

RgbColor red(colorSaturation, 0, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor black(0, 0, 0);
RgbColor white(125, 125, 125);

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

    // Turn first half red
    for (int count = 0; count <= median; count++) {
      strip.SetPixelColor(count, red);
      delay(5);
      strip.Show();
    }

    // Turn second half blue
    for (int count = median; count <= PixelCount; count++) {
      strip.SetPixelColor(count, blue);
      delay(5);
      strip.Show();
    }
}
void TurnAllOff() {
  for (int count = 0; count <= PixelCount; count++) {
    strip.SetPixelColor(count, white); 
  }   
    strip.Show();
}
void loop() {
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
  TurnAllOff();
  delay(200);
  
  // Set first half of strip
  for (int count = 0; count <= median; count++) {
    strip.SetPixelColor(count, firstColor);
  }

  // Set second half of strip
  for (int count = median; count <= PixelCount; count++) {
    strip.SetPixelColor(count, secondColor);
  }

    strip.Show();
    delay(300);
}
