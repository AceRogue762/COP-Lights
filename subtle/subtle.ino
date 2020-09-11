#include <NeoPixelBus.h>

const uint16_t PixelCount = 245;
const uint16_t PixelPin = 4;
const int brightness = 70;
int count = 0;

int LightningFrequency = 20;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor blue(0, 0, brightness);
RgbColor white(5);
RgbColor black(0);
RgbColor yellow(brightness, brightness, 0);

void setup() {
   Serial.begin(115200);

   strip.Begin();
   strip.Show();

   for (int count = 0; count <= PixelCount; count++) {
      strip.SetPixelColor(count, white);
      delay(5);
      strip.Show();
    }

}

void SetAllPixels(RgbColor color) {
   for (int count = 0; count <= PixelCount; count++) {
      strip.SetPixelColor(count, color); 
   }   
    
   strip.Show();
}

void loop() {
  
  if (count % LightningFrequency == 0) {
    // Single lightning flash
    SetAllPixels(black);
    
    SetAllPixels(yellow);
  
    strip.Show();
    delay(50);
    
    SetAllPixels(white); 
  }

  if (count % LightningFrequency * 2 == 0) {
    // Double lightning flash
    SetAllPixels(black);
    
    SetAllPixels(yellow);
  
    strip.Show();
    delay(50);
    
    SetAllPixels(white); 
    strip.Show();
    delay(10);

    SetAllPixels(black);
    
    SetAllPixels(yellow);
  
    strip.Show();
    delay(50);
    
    SetAllPixels(white); 

    count = 0;
  }
  
  int Pixel1Index = random(PixelCount);
  int Pixel2Index = random(PixelCount);
  int Pixel3Index = random(PixelCount);
  
  for (int count = brightness; count >= 15; count--) {
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
