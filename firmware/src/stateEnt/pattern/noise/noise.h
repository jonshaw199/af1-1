#ifndef PATTERN_NOISE_NOISE_H_
#define PATTERN_NOISE_NOISE_H_

#include <FastLED.h>

#include "stateEnt/pattern/pattern.h"

class Noise : public Pattern
{
  static void fillNoise8();
  static void setTargetPalette();

public:
  void setup();
  void preStateChange(int s);
};

#endif