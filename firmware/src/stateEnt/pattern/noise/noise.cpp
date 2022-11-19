#include "noise.h"

namespace noise
{
  CRGB *leds;
  CRGBPalette16 currentPalette;
  TBlendType currentBlending;
  CRGBPalette16 targetPalette;

  CHSV orange = CHSV(10, 255, 225);
  CHSV purple = CHSV(200, 250, 150); // 200, 150
}

using namespace noise;

void Noise::setup()
{
  Pattern::setup();

  leds = new CRGB[CNT];

#if CNT
#if CNT_A
  FastLED.addLeds<LED_TYPE_A, LED_PIN_A, LED_ORDER_A>(leds, CNT);
#endif
#if CNT_B
  FastLED.addLeds<LED_TYPE_B, LED_PIN_B, LED_ORDER_B>(leds, CNT);
#endif
  // FastLED.setBrightness(10);
  // FastLED.showColor(CRGB::DarkRed);
#endif

  currentPalette = CRGBPalette16(orange);
  setTargetPalette();
  set(Event(
      "Noise",
      [](ECBArg a)
      { fillNoise8();
        FastLED.show(); },
      EVENT_TYPE_TEMP, 1));

  set(Event(
      "Noise_Blend",
      [](ECBArg a)
      {
        nblendPaletteTowardPalette(currentPalette, targetPalette, 48);          // Blend towards the target palette over 48 iterations.
        FastLED.show(); },
      EVENT_TYPE_TEMP, 10));

  set(Event(
      "Noise_MovingTarget",
      [](ECBArg a)
      { setTargetPalette(); },
      EVENT_TYPE_TEMP, 5000));
}

void Noise::preStateChange(int s) {
  delete[] leds;
}

void Noise::fillNoise8()
{

#define scale 5 // Don't change this programmatically or everything shakes.

  for (int i = 0; i < CNT; i++)
  {                                                                                        // Just ONE loop to fill up the LED array as all of the pixels change.
    uint8_t index = inoise8(i * scale, getCurStateEnt()->getElapsedMs() / 10 + i * scale); // Get a value from the noise function. I'm using both x and y axis.
    leds[i] = ColorFromPalette(currentPalette, index, 255, LINEARBLEND);                   // With that value, look up the 8 bit colour palette value and assign it to the current LED.
  }

} // fillnoise8()

void Noise::setTargetPalette()
{
  CHSV o[] = {orange, purple};
  uint16_t seed = getCurStateEnt()->getElapsedMs() / 1000;
  random16_set_seed(seed);
  Serial.print("Random nums (seed: ");
  Serial.print(seed);
  Serial.print("): ");
  uint8_t randOne = random8(2);
  uint8_t randTwo = random8(2);
  uint8_t randThree = random8(2);
  uint8_t randFour = random8(2);
  Serial.print(randOne);
  Serial.print(randTwo);
  Serial.print(randThree);
  Serial.println(randFour);
  targetPalette = CRGBPalette16(o[randOne], o[randTwo], o[randThree], o[randFour]);

  /*
  uint8_t baseC = random8();
  targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 255, random8(128, 255)), // Create palettes with similar colours.
                                CHSV(baseC + random8(64), 255, random8(128, 255)),
                                CHSV(baseC + random8(96), 192, random8(128, 255)),
                                CHSV(baseC + random8(16), 255, random8(128, 255)));
                                */
}
