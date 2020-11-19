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
 * 
 */
 
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <esp_wifi.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>
#include <esp_now.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "config.h"
#include "espnow.h"
#include "animations.h"

// Create webserver and DNS server objects
AsyncWebServer webServer(SERVER_PORT);
DNSServer dnsServer;

// Create websocket connection for color streams
AsyncWebSocket socket("/api/stream");

// Create the WiFiManager object
AsyncWiFiManager wifiManager(&webServer, &dnsServer);

// Task handler for animation
TaskHandle_t currentTaskHandler = NULL;

// This device's broadcasted MAC address
uint8_t broadcastAddress[] = {0xAC, 0x67, 0xB2, 0x2A, 0x7A, 0x84};

// The audio extension's MAC address
uint8_t audioAddress[] = {0x2F, 0x0E, 0xAC, 0x2B, 0x3C, 0x8F};

// Global copy of audio device connection and pair status
esp_now_peer_info_t audioextension;
bool audioPaired = false;

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
 * Called when entering WiFi config mode
 */
void configModeCallback (AsyncWiFiManager *wifiManager) {
  // Set status LED to yellow
  strip.SetPixelColor(0, RgbColor(255, 255, 0));
  strip.Show();

  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(wifiManager -> getConfigPortalSSID());
}

/**
 * Connect to preconfigured WiFi network using ESPAsyncWiFiManager.
 * If there is no network configured, an access point is created.
 */
void connectWifi() {
  // Set custom MAC address
  esp_wifi_set_mac(ESP_IF_WIFI_STA, &broadcastAddress[0]);
  
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    // Set status LED to red
    strip.SetPixelColor(0, RgbColor(255, 0, 0));
    strip.Show();
    
    Serial.println("WiFi connection failed");

    if(BLOCK_UNTIL_CONNECTED) {
      Serial.print("Retrying in ");
      Serial.print(CONNECT_TIMEOUT);
      Serial.println(" seconds");
      
      delay(CONNECT_TIMEOUT * 1000);
      
      while (!wifiManager.autoConnect()) {
        Serial.println("Retrying...");
        delay(CONNECT_TIMEOUT * 1000); 
      }
    } else {
      return;
    }
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  strip.SetPixelColor(0, RgbColor(0, 255, 0));
  strip.Show();

  // Optionally start mDNS responder
  if (USE_MDNS)
    startMDNS();
}

/*  *  *  *  *  *  *  *  *  *  * WiFi *  *  *  *  *  *  *  *  *  */

/*  *  *  *  *  *  *  *  *  *  * ESPNow *  *  *  *  *  *  *  *   */

// Initialize ESPNow with fallback
void initESPNow() {
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow initialized");
  } else {
    Serial.println("ESPNow initialization failed. Retrying.");
    ESP.restart();
  }

  // Register send and receive callbacks
  esp_now_register_recv_cb(onDataReceived);
  esp_now_register_send_cb(onDataSent);
}

// ESPNow data received callback
void onDataReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  memcpy(&currentFFT, data, sizeof(currentFFT));

  for (int i = 0; i < 8; i++) {
    Serial.println(currentFFT.FFTBands[i]);
  }
}

// ESPNow data send callback
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

/*  *  *  *  *  *  *  *  *  *  * ESPNow *  *  *  *  *  *  *  *   */

/*  *  *  *  *  *  *  *  *  *  * mDNS *  *  *  *  *  *  *  *  *  */

/**
 * Start the mDNS responder to allow browsing to [hostname].local
 * rather than IP address
 */
void startMDNS()
{
  if (!MDNS.begin(HOSTNAME)) {
    Serial.println("Error setting up mDNS responder");
  }
  
  Serial.println("mDNS responder started");
}

/*  *  *  *  *  *  *  *  *  *  * mDNS *  *  *  *  *  *  *  *  *  */

/*  *  *  *  *  *  *  *  *  *  * OTA  *  *  *  *  *  *  *  *  *  */

/**
 * Start the OTA update server
 */
void startOTA() {
  ArduinoOTA.onStart([]() {
    Serial.println("OTA start");

    // End the current animation task
    if(currentTaskHandler != NULL) {
      vTaskDelete(currentTaskHandler);
      currentTaskHandler = NULL;
    }
    
    setAllPixels(black);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA End");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    int percentDone = progress / (total / 100);
    Serial.printf("OTA Progress: %u%%\r", percentDone);
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
  { "/api/reset",             HTTP_GET, &handleReset           },
  { "/api/status",            HTTP_GET, &handleGetStatus       },
  { "/api/power/on",          HTTP_GET, &handlePowerOn         },
  { "/api/power/off",         HTTP_GET, &handlePowerOff        },
  { "/api/effects/set",       HTTP_GET, &handleSetEffect       },
  { "/api/power/toggle",      HTTP_GET, &handlePowerToggle     },
  { "/api/animations/get",    HTTP_GET, &handleGetAnimations   },
  { "/api/animations/select", HTTP_GET, &handleSelectAnimation },
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

  // Link favicon
  webServer.on("/favicon.gif", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/favicon.gif", "text/javascript");
  });

  // Add the CORS header, if enabled
  if (CORS_ENABLED) {
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  }

  // Set up the websocket endpoint
  socket.onEvent(handleSocketEvent);
  webServer.addHandler(&socket);

  // Finally, start the configured web server
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

/**
 * Returns a 403 "forbidden" error to the client
 */
void handleForbidden(AsyncWebServerRequest *request) {
  String message = "403 Forbidden\n\n";
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

/*  *  *  *  *  *  *  *  *  *  * Web Socket *  *  *  *  *  *  *  *  * */

/**
 * Handles all incoming messages over the websocket connection
 */
void handleSocketEvent(
  AsyncWebSocket * server, 
  AsyncWebSocketClient * client, 
  AwsEventType type, 
  void * arg, 
  uint8_t *data, 
  size_t len
){
  if(type == WS_EVT_CONNECT){
    // Client connected
    client->text("Hello from your kickass LED strip");
  } else if(type == WS_EVT_DISCONNECT){
    // Client disconnected
    Serial.println("Client disconnected");
  }
}

/*  *  *  *  *  *  *  *  *  *  * Web Socket *  *  *  *  *  *  *  *  * */

/*  *  *  *  *  *  *  *  *  *  * Web Socket *  *  *  *  *  *  *  *  * */

/*  *  *  *  *  *  *  *   *  Route Handlers *  *  *  *  *  *  *  *   */

/**
 * Serve the client React application
 */
void handleRoot(AsyncWebServerRequest *request) {
  request -> send(SPIFFS, "/index.html", "text/html");
}

/**
 * Perform a software reset
 */
void handleReset(AsyncWebServerRequest *request) {
  ESP.restart();
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
 * API endpoint to set an effect (color/animation)
 */
void handleSetEffect(AsyncWebServerRequest *request) {
  int params = request -> params();

  String type;
  int r, g, b;

  // End the current animation task
  if(currentTaskHandler != NULL) {
    vTaskDelete(currentTaskHandler);
    currentTaskHandler = NULL;
  }

  // Parse request
  for(int i=0; i < params; i++) {
    AsyncWebParameter* parameter = request -> getParam(i);

    if (parameter -> name() == "type") {
      // Got effect type. Validate it.
      type = parameter -> value();
    }

    if (parameter -> name() == "r") {
      // Got red color component
      r = parameter -> value().toInt();
    }

    if (parameter -> name() == "g") {
      // Got green color component
      g = parameter -> value().toInt();
    }

    if (parameter -> name() == "b") {
      // Got blue color component
      b = parameter -> value().toInt();
    }
  }

  // Construct the effect color from request parameters
  RgbColor effectColor = RgbColor(r, g, b);

  // Show the color
  setAllPixels(effectColor);

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

  // Initialize ESPNow
  initESPNow();

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
