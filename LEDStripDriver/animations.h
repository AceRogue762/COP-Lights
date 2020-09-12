#ifndef ANIMATIONS_H
#define ANIMATIONS_H

/**
 * Header file to define objects to wrap animation data and
 * handler functions.
 * 
 * When adding a new animation, add 1 to animationCount, add an 
 * entry to animationTable and create a void function implementing 
 * the new animation in animationFunctions.h.
 */

#include "animationFunctions.h"
#include "Arduino.h"

// Current number of animations. Increment by one when adding
// an animation
const int animationCount = 4;

// Animation mapping table lookup entry
typedef struct animationTableEntry {
  int id;            // id for the animation
  const char * name; // name for the animation
  void (*handler)(void *); // handler function
} animationTableEntry;


// Animation mapping table. Add new animations here.
animationTableEntry animationTable[animationCount+1] =
{ 
  { 1, "Cop Lights Alternating", &copLightsAlternating },
  { 2, "Cop Lights Line Out",    &copLightsLineOut },
  { 3, "Cop Lights Mix",         &copLightsMix },
  { 4, "Rainy Day",              &rainyDay },
  { NULL }
};


#endif
