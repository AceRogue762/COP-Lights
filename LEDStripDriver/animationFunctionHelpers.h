#ifndef ANIMATIONFUNCTIONHELPERS_H
#define ANIMATIONFUNCTIONHELPERS_H

/**
 * Helper functions and objects to initialize the strip and 
 * simplify common animation tasks. 
 */

#include "Arduino.h"

#include <NeoPixelBrightnessBus.h>
#include <NeoPixelSegmentBus.h>
#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

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

/** 
 * Initialize the NeoPixel interface
 */
void initLEDs() {
  strip.Begin();
  strip.Show();
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


/*  *  *  *  *  *  *  *  *  NeoPixelAnimator support  *  *  *  *  *  *  *  *  *  */

const uint8_t AnimationChannels = 1; // we only need one as all the pixels are animated at once
NeoPixelAnimator animations(AnimationChannels); // NeoPixel animation management object
boolean fadeToColor = true;  // general purpose variable used to store effect state

/**
 * What is stored for state is specific to the need, in this case, the colors.
 * Basically whatever you need inside the animation update function
 */
struct MyAnimationState
{
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// One entry per pixel to match the animation timing manager
MyAnimationState animationState[AnimationChannels];

/**
 * Sets a random seed. Used by some NeoPixelAnimater animations
 */
void SetRandomSeed() {
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    randomSeed(seed);
}

/**
 * Simple blend function for NeoPixelBusAnimator
 */
void BlendAnimUpdate(const AnimationParam& param) {
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        animationState[param.index].StartingColor,
        animationState[param.index].EndingColor,
        param.progress);

    // apply the color to the strip
    for (uint16_t pixel = START_LED; pixel < LED_COUNT; pixel++)
    {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

/**
 * Looping fade in/out animation
 */
void FadeInFadeOutRinseRepeat(float luminance) {
    if (fadeToColor)
    {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = RgbColor(140,0, 0);
        RgbColor secondary = RgbColor(175, 175, 175);
        uint16_t time = random(400, 500);

        animationState[0].StartingColor = secondary;
        animationState[0].EndingColor = target;

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }
    else 
    {
        // fade to black
        uint16_t time = random(400, 500);

        animationState[0].StartingColor = RgbColor(175, 175, 175);
        animationState[0].EndingColor = RgbColor(0, 0, 175);

        animations.StartAnimation(0, time, BlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}

/*  *  *  *  *  *  *  *  *  NeoPixelAnimator support  *  *  *  *  *  *  *  *  *  */

#endif
