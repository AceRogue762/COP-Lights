#ifndef _CONFIG_H
#define _CONFIG_H

/**
 * Configuration parameters used by the application
 */

// Debug mode (comment out to disable serial logging)
#define DEBUG

// Number of bytes in flash memory to reserve for simple
// persistent storage
#define EEPROM_SIZE 2

// WiFi settings
const char * HOSTNAME     = "LEDStrip";     // Network device name
const uint8_t CONNECT_TIMEOUT = 5; // Timeout for connection (seconds)
bool BLOCK_UNTIL_CONNECTED = true; // Whether to block execution until connected

// Web server settings
bool CORS_ENABLED = true; // True value adds the Access-Control-Allow-Origin header

// NeoPixel strip settings
const uint8_t LED_PIN    = 4;   // GPIO pin connected to LED data
const uint8_t LED_COUNT  = 246; // Number of LEDs in the strip
const uint8_t START_LED  = 1;   // Index of the first pixel (offset by one if using internal status LED)
const uint8_t SATURATION = 128; // Maximum brightness

// Network settings
#define SERVER_PORT 80  // Port for web application

#endif
