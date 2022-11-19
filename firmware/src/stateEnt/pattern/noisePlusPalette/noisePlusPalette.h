#ifndef PATTERN_NOISEPLUSPALETTE_NOISEPLUSPALETTE_H_
#define PATTERN_NOISEPLUSPALETTE_NOISEPLUSPALETTE_H_

#include <FastLED.h>

#include "stateEnt/pattern/pattern.h"

class NoisePlusPalette : public Pattern
{
  static void fillnoise8();
  static void mapNoiseToLEDsUsingPalette();
  static void plebLoop();
  static void ChangePaletteAndSettingsPeriodically();
  static void SetupRandomPalette();
  static void SetupBlackAndWhiteStripedPalette();
  static void SetupPurpleAndGreenPalette();
  static void setupShawHalloweenPalette();
  static uint16_t XY(uint8_t x, uint8_t y);

public:
  void setup();
  void preStateChange(int s);
};

#endif