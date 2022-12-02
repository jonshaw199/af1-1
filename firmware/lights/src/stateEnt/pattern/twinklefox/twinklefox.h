#ifndef PATTERN_TWINKLEFOX_TWINKLEFOX_H_
#define PATTERN_TWINKLEFOX_TWINKLEFOX_H_

#include <FastLED.h>

#include "stateEnt/pattern/pattern.h"

class Twinklefox : public Pattern
{
  static CRGB *leds;
  static CRGBPalette16 gCurrentPalette;
  static CRGBPalette16 gTargetPalette;
  // Background color for 'unlit' pixels
  // Can be set to CRGB::Black if desired.
  static CRGB gBackgroundColor;
  // Example of dim incandescent fairy light background color
  // CRGB gBackgroundColor = CRGB(CRGB::FairyLight).nscale8_video(16);

  static void drawTwinkles(CRGB *leds);
  static CRGB computeOneTwinkle(uint32_t ms, uint8_t salt);
  static uint8_t attackDecayWave8(uint8_t i);
  static void coolLikeIncandescent(CRGB &c, uint8_t phase);
  static void chooseNextColorPalette(CRGBPalette16 &pal);

public:
  void setup();
  void preStateChange(int s);
};

#endif