# COP-Lights
Multiple LED scripts to display coding/animation capabilites of a NeoPixel Strip/Arduino set up

## Animations
1. The first animation consists of two colors swapping halves of the light strip while a series of flashes occur quickly between animations (red and blue with white flahes in this case).
2. The second animation consists of three total colors. One starts at each end of the light strip while the third is centered and given its own function. The end lights move pixel by pixel to the median while the third color in the median moves pixel by pixel in both outward directions to meet the other two (red and blue on the outsides to meet White in the median).
3. The third animation consists of a fade-in-fade-out-based animation with two main colors that loop through the animation quickly to give a flashing look, while a third and/or fourth color are animated for an extremely short period of time to give the effect of a sub flashing color (red and blue are faded in and out quickly while the color white supplments as a third color istead of blacking out the pixels).
4. The fourth animation consists of an animation being sent to random pixels with a delay that is meant to look like rain and random flashes of "lightning."

## Setup
### Prerequisites
1. ESP32 development board. [This one](https://www.amazon.com/IZOKEE-ESP-WROOM-32-Development-Microcontroller-Compatible/dp/B08151X2FZ/ref=sr_1_1?dchild=1&keywords=IZOKEE+Development+Board+for+ESP32&qid=1600120004&s=electronics&sr=1-1) is cheap and works well.
2. A NeoPixel strip, such as [this one](https://www.amazon.com/gp/product/B018XAELE4/ref=ppx_yo_dt_b_asin_title_o06_s01?ie=UTF8&psc=1).
3. An adequate power supply. For the above strip of 300 pixels, [this one](https://www.amazon.com/15A-Power-Supply-100V-240V-110V/dp/B07K9Q4DV1/ref=sr_1_1_sspa?dchild=1&keywords=5v+15a+power+supply&qid=1600120143&sr=8-1-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUE1TUc2N01YOVdPOTYmZW5jcnlwdGVkSWQ9QTA2Mzk0MzIxR0pEQlVWOVVUSjJCJmVuY3J5cHRlZEFkSWQ9QTAxMzE2NDkxREdaV0xVQTNOVTNCJndpZGdldE5hbWU9c3BfYXRmJmFjdGlvbj1jbGlja1JlZGlyZWN0JmRvTm90TG9nQ2xpY2s9dHJ1ZQ==) is sufficient. Refer to [Adafruit's documentation](https://learn.adafruit.com/adafruit-neopixel-uberguide/powering-neopixels) to calculate how much power you will need for different strips.
4. [Arduino IDE](https://www.arduino.cc/en/main/software)

### Hardware Setup
Refer to the [ESP32 pinout](https://images-na.ssl-images-amazon.com/images/I/71M-2-jhcEL._AC_SL1001_.jpg) for this section.

1. Plug the power supply's + lead to the + pin on the NeoPixel strip, and the - lead to the - pin
2. Connect the strip's DATA pin to pin 4 on the ESP32
3. Connect the power supply to the VIN and GND pins on the ESP32
3. Plug the ESP32 into your PC using a micro USB cable

### Arduino IDE Setup
1. Install the [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) library. This is used to provide the asynchronous web server and client application.
2. Install the [ESP32 Sketch Data Upload Plugin](https://github.com/me-no-dev/arduino-esp32fs-plugin). This is used to upload the web application to the ESP32's filesystem.
1. Open LEDDriver/LEDDriver.ino in the Arduino IDE
2. Open config.h
3. Set SSID and PASSWORD to your WiFi network's SSID and password
4. Click upload. You may need to press the BOOT button on the ESP32 to connect.
5. Open the Serial Monitor to obtain your device's IP address
6. Browse to your device's IP address in a web browser
