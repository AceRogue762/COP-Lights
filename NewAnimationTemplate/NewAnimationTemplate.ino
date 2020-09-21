/**
 * Template sketch for new creating new animations. Copy this file
 * and build your new animation off of it to retain WiFi and OTA
 * functionality while testing.
 * 
 * - Edit config.h to set your WiFi network details
 * - Edit 'animationFunction' below with your new animation code
 * - Upload and test until satisfied
 * - Optional: Save tthis sketch in a new folder and give it a 
 *   name
 * - Put the contents of animationFunction into a new function in 
 *   LEDStripDriver/animationFunctions.h
 * - Update animationTable in LEDStripDriver/animations.h
 */

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelSegmentBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>
#include <ArduinoOTA.h>
#include <WiFi.h>

#include "config.h"

// Create RGB colors to be used by animations
RgbColor red    (SATURATION, 0, 0);
RgbColor green  (0, SATURATION, 0);
RgbColor blue   (0, 0, SATURATION);
RgbColor pink   (SATURATION, 0, SATURATION);
RgbColor yellow (SATURATION, SATURATION, 0);
RgbColor orange (SATURATION, SATURATION/2, 0);
RgbColor purple (SATURATION/2, 0, SATURATION);
RgbColor white  (SATURATION);
RgbColor black  (0);

// Create the NeoPixelBus strip object
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);

// Task handler for animation
TaskHandle_t currentTaskHandler = NULL;

/**
 * Write your new animation in this function
 */
void animationFunction(void * pvParameters) {
  (void) pvParameters;

  // You can run any setup or a startup animation here
  
  // Main animation loop
  while (true) {
    
  }
}

/**
 * Use this function to set all pixels to a color
 */
void setAllPixels(RgbColor color) {
  for (int count = START_LED; count <= LED_COUNT; count++) {
    strip.SetPixelColor(count, color); 
  }   
  
  strip.Show();
}

/** 
 * Initialize the NeoPixel interface
 */
void initLEDs() {
  strip.Begin();
  strip.Show();
}


/*  *  *  *  *  *  *  *  *  *  * WiFi *  *  *  *  *  *  *  *  *  */

/**
 * Connect to preconfigured WiFi network. Optionally blocks
 * until connected.
 */
void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(NETWORK_SSID, NETWORK_PASS);
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("WiFi connection failed!");

      strip.SetPixelColor(0, RgbColor(255, 0, 0));
      strip.Show();

      if(BLOCK_UNTIL_CONNECTED) {
        Serial.print("Retrying in ");
        Serial.print(CONNECT_TIMEOUT);
        Serial.println(" seconds");
        
        delay(CONNECT_TIMEOUT * 1000);
        
        while(WiFi.waitForConnectResult() != WL_CONNECTED) {
          Serial.println("Retrying...");
          WiFi.begin(NETWORK_SSID, NETWORK_PASS);
          
          delay(CONNECT_TIMEOUT * 1000); 
        }
      } else {
        return;
      }
      
      
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  strip.SetPixelColor(0, RgbColor(0, 255, 0));
  strip.Show();
}

/*  *  *  *  *  *  *  *  *  *  * WiFi *  *  *  *  *  *  *  *  *  */

/*  *  *  *  *  *  *  *  *  *  * OTA  *  *  *  *  *  *  *  *  *  */

/**
 * Start the OTA update server
 */
void startOTA() {
  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setHostname(HOSTNAME);
  ArduinoOTA.begin();
  

  Serial.println("OTA ready");
}

/*  *  *  *  *  *  *  *  *  *  * OTA  *  *  *  *  *  *  *  *  *  */

/*  *  *  *  *  *  *  *   * Animation Code  *  *  *  *  *  *  *  */

/*
 * Create a task to run the animation 
 */
void runAnimation() {
  // Create new task to run the animation
  xTaskCreate(
    animationFunction,
    "New Animation",
    1024, // Stack size (bytes)
    NULL, // Parameter to pass
    1,    // Task priority (high)
    &currentTaskHandler  // Task handle
  );
}

/*  *  *  *  *  *  *  *   * Animation Code  *  *  *  *  *  *  *  */


void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  Serial.println("Booting");

  // Initialize the strip
  initLEDs();

  // Run the animation
  runAnimation();

  // Connect to WiFi
  connectWifi();
}

void loop() {
  // OTA updates must be handled in the main thread. All other 
  // execution is task-based.
  ArduinoOTA.handle();
}
