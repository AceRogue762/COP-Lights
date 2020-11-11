#ifndef _ESPNOW_H
#define _ESPNOW_H

/**
 * Data structures and methods to handle ESPNow communication
 * between the controller and extension devices.
 */

#include "Arduino.h"

// Wrapper for a single message received from the controller
typedef struct message {
  int FFTBands[8]; // Frequency band values (0 - 12)
} message;

message currentFFT;

#endif
