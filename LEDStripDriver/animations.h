#ifndef ANIMATIONS_H
#define ANIMATIONS_H

/**
 * Header file to define objects to wrap animation data and
 * handler functions.
 * 
 * When adding a new animation, add an entry to animationTable 
 * and create a void function implementing the new animation in 
 * animationFunctions.h.
 */

#include "animationFunctions.h"
#include "Arduino.h"

// Animation mapping table lookup entry
typedef struct animationTableEntry {
  int id;            // id for the animation
  const char * name; // name for the animation
  void (*handler)(void *); // handler function
} animationTableEntry;


// Animation mapping table. Add new animations here.
animationTableEntry animationTable[3] =
{ 
  { 1, "Cop Lights Flashing", &copLightsFlashing },
  { 2, "Rainy Day",           &rainyDay },
  { NULL }
};


#endif
