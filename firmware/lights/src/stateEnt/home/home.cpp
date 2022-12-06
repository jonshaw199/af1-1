#include "home.h"

String Home::getName()
{
    return "Home";
}

void Home::setup()
{
    LightsBase::setup();

    currentPaletteIndex = 0;
    currentPalette = CRGBPalette16(CRGB::Black);
    targetPalette = CRGBPalette16(CRGB::Black);

    // Turn off lights
#if CNT
    FastLED.showColor(CRGB::Black);
#endif
}
