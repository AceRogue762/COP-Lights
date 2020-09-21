/**
 * Main sketch for ESP32-controlled NeoPixel strip.
 * 
 * ~ NeoPixel Animations ~
 * Animations are run one at a time in a FreeRTOS task. Each entry in 
 * animationTable corresponds to an animation and its handler function.
 * 
 * ~ Web App ~
 * ReactJS client application served at /
 * Animations API served at /animations
 *  - /animations/get: Returns all available animations
 *  - /animations/select?id=[int animationId]: Start playing the animation
 *    with id animationId
 * 
 * ~ Configuration ~
 * Edit config.h to set NeoPixel, WiFi and web server parameters.
 * See animations.h for adding new animations.
 */

#include <ESPAsync_WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "config.h"
#include "animations.h"

// Create webserver object
AsyncWebServer webServer(SERVER_PORT);

// Task handler for animation
TaskHandle_t currentTaskHandler = NULL;

/*  *  *  *  *  *  *  *  *  * System Status *  *  *  *  *  *  *  */

/**
 * Struct to hold current system status
 */
struct systemStatus {
  bool powerOn; // whether the LED strip is powered on
  unsigned short int selectedAnimationId; // id of the the currently selected animation
};

// Current system status
systemStatus currentStatus = {
  false, 0
};

/**
 * Function to set status
 */
void setSystemStatus(bool powerOn, int selectedAnimationId) {
  currentStatus.powerOn = powerOn;
  currentStatus.selectedAnimationId = selectedAnimationId;
}

/**
 * Function to return the current status as a JSON string
 */
String getSystemStatus() {
  String message = "{\"status\": {";
    message += "\"powerOn\": " + String(currentStatus.powerOn) + ", ";
    message += "\"selectedAnimationId\": " + String(currentStatus.selectedAnimationId);
  message += "}}";

  return message;
}

/*  *  *  *  *  *  *  *  *  * System Status *  *  *  *  *   *  *  */


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


/*  *  *  *  *  *  *  *  *  *  * Web Server *  *  *  *  *  *  *  *  * */

/**
 * Endpoint table lookup entry
 */
struct endpointTableEntry {
  const char * endpoint;                    // endpoint string
  WebRequestMethod allowedMethod;           // allowed method
  void (*handler)(AsyncWebServerRequest *); // handler function pointer
};

/**
 * Lookup table to map each endpoint to its 
 * allowed method and handler function
 */
static struct endpointTableEntry endpointTable[] = 
{ 
  { "/",                      HTTP_GET, &handleRoot            },
  { "/api/status",            HTTP_GET, &handleGetStatus       },
  { "/api/power/on",          HTTP_GET, &handlePowerOn         },
  { "/api/power/off",         HTTP_GET, &handlePowerOff        },
  { "/api/power/toggle",      HTTP_GET, &handlePowerToggle     },
  { "/api/animations/select", HTTP_GET, &handleSelectAnimation },
  { "/api/animations/get",    HTTP_GET, &handleGetAnimations   },
  { NULL }
};

/**
 * Set up all endpoints/pages and start the web server
 */
void startWebServer() {
  Serial.println("Starting web server");

  // Add all enpoints from the table
  struct endpointTableEntry *thisEndpointEntry = endpointTable;
  for ( ; thisEndpointEntry -> endpoint != NULL ; thisEndpointEntry++ )
    webServer.on(
    thisEndpointEntry -> endpoint, 
    thisEndpointEntry -> allowedMethod, 
    [](AsyncWebServerRequest *request) {
        
      handleRequest(request);
    });

  // Link supporting javascript
  webServer.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.js", "text/javascript");
  });

  // Add the CORS header, if enabled
  if (CORS_ENABLED) {
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  }
    
  webServer.onNotFound(handleNotFound);
  webServer.begin();

  Serial.println("Web server ready");
}

/**
 * Validates all incoming requests, and decides which handler to call
 */
void handleRequest(AsyncWebServerRequest *request) {
  digitalWrite(LED_BUILTIN, 1);

  String endpoint = request -> url();
  struct endpointTableEntry *thisEndpointEntry = endpointTable;

  // Look up the endpoint
  for ( ; thisEndpointEntry -> endpoint != NULL ; thisEndpointEntry++ ) {
    if ( endpoint == thisEndpointEntry -> endpoint ) {
      if ( request -> method() == thisEndpointEntry -> allowedMethod ) {
        // Call the endpoint's handler function
        thisEndpointEntry -> handler(request);
      } else {
        // 405
        handleWrongMethod(request);
      }
    }
  }   

  // Endpoint does not exist
  handleNotFound(request);

  digitalWrite(LED_BUILTIN, 0);
}

/**
 * Returns a 404 "not found" error to the client
 */
void handleNotFound(AsyncWebServerRequest *request) {
  // Respond to OPTIONS method for CORS pre-flight
  if (request->method() == HTTP_OPTIONS) {
    request -> send(200);
    return;
  }
  
  String message = "404 Not Found\n\n";
  message += "URI: ";
  message += request -> url();
  message += "\nMethod: ";
  message += (request -> method() == HTTP_GET) ? "GET" : "POST";
  message += "\nParameters: ";
  message += request -> params();
  message += "\n";
  
  for (uint8_t i = 0; i < request -> params(); i++) {
    AsyncWebParameter* param = request -> getParam(i);
    message += " " + param -> name() + ": " + param -> value() + "\n";
  }
  
  request -> send(404, "text/plain", message);
}

/**
 * Returns a 405 "method not allowed" error to the client
 */
void handleWrongMethod(AsyncWebServerRequest *request) {
  String message = "405 Method Not Allowed\n\n";
  message += "URI: ";
  message += request -> url();
  message += "\nMethod: ";
  message += (request -> method() == HTTP_GET) ? "GET" : "POST";
  message += "\nParameters: ";
  message += request -> params();
  message += "\n";
  
  for (uint8_t i = 0; i < request -> params(); i++) {
    AsyncWebParameter* param = request -> getParam(i);
    message += " " + param -> name() + ": " + param -> value() + "\n";
  }
  
  request -> send(405, "text/plain", message);
}

/*  *  *  *  *  *  *  *  *  *  * Web Server *  *  *  *  *  *  *  *  * */

/*  *  *  *  *  *  *  *   *  Route Handlers *  *  *  *  *  *  *  *   */

/**
 * Serve the client React application
 */
void handleRoot(AsyncWebServerRequest *request) {
  request -> send(SPIFFS, "/index.html", "text/html");
}

/**
 * API endpoint to get current status
 */
void handleGetStatus(AsyncWebServerRequest *request) {
  request -> send(200, "text/json", getSystemStatus());
}

/**
 * API endpoint to power on the LED strip
 */
void handlePowerOn(AsyncWebServerRequest *request) {
  // Ignore if already on
  if (currentStatus.powerOn) {
    request -> send(200, "text/json", getSystemStatus());
    return;
  }

  // Power on
  currentStatus.powerOn = true;

  // Restart animation
  int animationId = currentStatus.selectedAnimationId;

  if (animationId > 0) {
    xTaskCreate(
      animationTable[animationId-1].handler,
      animationTable[animationId-1].name,
      1024, // Stack size (bytes)
      NULL, // Parameter to pass
      1,    // Task priority (high)
      &currentTaskHandler  // Task handle
    );
  }

  // Send the response
  request -> send(200, "text/json", getSystemStatus());
}

/**
 * API endpoint to power off the LED strip
 */
void handlePowerOff(AsyncWebServerRequest *request) {
  // Ignore if already off
  if (! currentStatus.powerOn) {
    request -> send(200, "text/json", getSystemStatus());
    return;
  }
  
  // End the current animation task
  currentStatus.powerOn = false;
  if(currentTaskHandler != NULL) {
    vTaskDelete(currentTaskHandler);
    currentTaskHandler = NULL;
  }

  // Black out all pixels
  setAllPixels(black);

  // Send the response
  request -> send(200, "text/json", getSystemStatus());
}

/**
 * API endpoint to toggle power
 */
void handlePowerToggle(AsyncWebServerRequest *request) {
  if (currentStatus.powerOn) {
    handlePowerOff(request);
  } else {
    handlePowerOn(request);
  }
}

/**
 * API endpoint to select an animation
 */
void handleSelectAnimation(AsyncWebServerRequest *request) {
  int params = request -> params();
  
  for(int i=0;i<params;i++) {
    AsyncWebParameter* parameter = request -> getParam(i);

    int animationId;

    if (parameter -> name() == "id") {
      // Found the ID parameter. Look up the selection.
      animationId = parameter -> value().toInt();
      struct animationTableEntry *thisAnimationEntry = animationTable;
      
      for ( ; thisAnimationEntry -> id != NULL ; thisAnimationEntry++ ) {
        if ( animationId == thisAnimationEntry -> id ) {
          // Valid animation
          if (animationId != currentStatus.selectedAnimationId)
            setCurrentAnimation(animationId);
          
          request -> send(
            200, 
            "text/json", 
            "{\"result\": {" 
              "\"id\": " + String(animationId) + ", "
              "\"name\": \"" + thisAnimationEntry -> name + "\", "
              "\"error\": null"
            "}}"
          );
    
          return;
        }
      }
    }
    
    // Invalid animation ID received
    request -> send(
      400, 
      "text/json", 
      "{\"result\": {"
        "\"id\": " + String(animationId) + ", "
        "\"error\": \"Invalid animation ID\""
      "}}"
    );
  }
}

/**
 * API endpoint to retrieve all available animations
 */
void handleGetAnimations(AsyncWebServerRequest *request) {
  struct animationTableEntry *thisAnimationEntry = animationTable;
  
  String message = "{\"animations\": [";
    for ( ; thisAnimationEntry -> id != NULL ; thisAnimationEntry++ ) {
      message += "{";
        message += "\"id\": " + String(thisAnimationEntry -> id) + ",";
        message += "\"name\": \"" + String(thisAnimationEntry -> name) + "\"";
      message += "},";
    }
  message.remove(message.length()-1);
  message += "]}";
  
  request->send(200, "text/json", message);
}

/*  *  *  *  *  *  *  *  *  *  * Route Handlers *  *  *  *  *  *  *   */

/*  *  *  *  *  *  *  *  *  *  * EEPROM *  *  *  *  *  *  *  *  *  *  */
/*
 * Retrieve the ID of the last selected animation from EEPROM
 */
void initEEPROMAndGetLastAnimation() {
  EEPROM.begin(EEPROM_SIZE);

  unsigned short int lastAnimationId = EEPROM.read(0);

  if (lastAnimationId == 255 || lastAnimationId == 0) {
    lastAnimationId = 0;
    Serial.println("No previous animation ID found");

    return;
  }

  Serial.print("Found saved animation ID: ");
  Serial.println(lastAnimationId);

  currentStatus.powerOn = true;
  setCurrentAnimation(lastAnimationId);
}

/*
 * Set the current animation, create a task to run it and 
 * save its id to flash memory
 */
void setCurrentAnimation(unsigned short int animationId) {
  if (animationId < 1) {
    Serial.println("Invalid animation ID. Ignoring.");
    return;
  }
  
  currentStatus.selectedAnimationId = animationId;

  // Save to flash memory
  EEPROM.write(0, animationId);
  EEPROM.commit();

  // Stop here if powered off
  if (! currentStatus.powerOn)
    return;

  // End the current animation task
  if (currentTaskHandler != NULL)
    vTaskDelete(currentTaskHandler);

  // Clear the strip
  setAllPixels(black);

  // Create new task to run the animation
  xTaskCreate(
    animationTable[animationId-1].handler,
    animationTable[animationId-1].name,
    1024, // Stack size (bytes)
    NULL, // Parameter to pass
    1,    // Task priority (high)
    &currentTaskHandler  // Task handle
  );
}

/*  *  *  *  *  *  *  *  *  *  * EEPROM *  *  *  *  *  *  *  *  *  *  */

/*  *  *  *  *  *  *  *  *  *  * SPIFFS *  *  *  *  *  *  *  *  *  *  */

/** 
 * Begin SPIFFS and check for formatting issues 
 */
void startSPIFFS() {
  SPIFFS.begin();

  // Look for valid home page HTML file
  if (!SPIFFS.exists("/index.html")) {
    // Not found, reformat
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    Serial.println("SPIFFS formatted");

    // Check to make sure formatting worked
    File f = SPIFFS.open("/index.html", "w");
    if (!f) {
        Serial.println("File open failed: index.html");
    } else {
        f.println("No webpage here. Did you upload your application data?");
    }
  } else {
    // All good
    Serial.println("SPIFFS is already formatted");
  }
}

/*  *  *  *  *  *  *  *  *  *  * SPIFFS *  *  *  *  *  *  *  *  *  *  */


void setup() {
  #ifdef DEBUG
  Serial.begin(115200);
  #endif
  
  Serial.println("Booting");

  // Initialize the NeoPixel interface
  initLEDs();

  // Start up the filesystem, format if needed
  startSPIFFS();

  // Load last animation, if one was set
  initEEPROMAndGetLastAnimation();
  
  // Connect to WiFi
  connectWifi();

  // Set up OTA updates
  startOTA();

  // Start the web server
  startWebServer();
}

void loop() {
  // OTA updates must be handled in the main thread. All other 
  // execution is task-based.
  ArduinoOTA.handle();
}
