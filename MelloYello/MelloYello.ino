#include <NeoPixelBus.h>

const uint16_t PixelCount = 245;
const uint16_t PixelPin = 4;
const uint16_t brightness = 100;
int count = 0;
int loopdirection = 0;


NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

RgbColor green(0, 15, 0);
RgbColor white(5);
RgbColor black(0);
RgbColor yellow(25, 25, 0);

void setup() {
   Serial.begin(115200);

   strip.Begin();
   strip.Show();

   for (int count = 0; count <= PixelCount; count++) {
      strip.SetPixelColor(count, green);
      delay(5);
      strip.Show();
    }
}

void loop() {
  if (count >= PixelCount) 
    loopdirection = 1;
    else if (count <= 0 && loopdirection == 1)
    loopdirection = 0;

   strip.SetPixelColor(count, black);
    
    if (loopdirection == 0) {
         int brightness = 25;
         for(int i = count; i >= count - 25; i--) {
         strip.SetPixelColor(i, RgbColor (brightness, brightness, 0));
         brightness--;
         } 
         
          strip.SetPixelColor(count - 25, green);
              count++;
   } else {
     int brightness = 25;
         for(int i = count; i <= count + 25; i++) {
         strip.SetPixelColor(i, RgbColor (brightness, brightness, 0));
         brightness--;
         } 
         
          strip.SetPixelColor(count + 25, green);
              count--;
    
   }
    
    strip.Show();
    delay(25);
}
