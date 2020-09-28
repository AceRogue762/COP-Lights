#ifndef ANIMATIONS_H
#define ANIMATIONS_H

/**
 * Header file to define objects to wrap animation data and
 * handler functions.
 * 
 * When adding a new animation, add an entry to animationTable 
 * and create a void function implementing the new animation in 
 * animationFunctions.h
 * 
 * id values should range from 1 to animationCount.
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
animationTableEntry animationTable[] =
{ 
  { 1, "Christmas Red Green Dance", &christmasRGDance     },
  { 2, "Cop Lights Alternating",    &copLightsAlternating },
  { 3, "Cop Lights Line Out",       &copLightsLineOut     },
  { 4, "Cop Lights Mix",            &copLightsMix         },
  { 5, "Halloween",                 &halloweenOrange      },
  { 6, "MelloYello",                &melloYello           },
  { 7, "Rainy Day",                 &rainyDay             },
  { NULL }
};


#endif
