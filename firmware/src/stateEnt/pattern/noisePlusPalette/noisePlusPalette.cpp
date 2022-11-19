#include <FastLED.h>

#include "noisePlusPalette.h"
#include "color/palette/hopegoddess_gp.h"
#include "color/palette/halloween_gp.h"

#define LED_PIN 3
#define BRIGHTNESS 96
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

// Params for width and height
const int kMatrixWidth = CNT;
const int kMatrixHeight = 1;

// Param for different pixel layouts
const bool kMatrixSerpentineLayout = true;

// This example combines two features of FastLED to produce a remarkable range of
// effects from a relatively small amount of code.  This example combines FastLED's
// color palette lookup functions with FastLED's Perlin noise generator, and
// the combination is extremely powerful.
//
// You might want to look at the "ColorPalette" and "Noise" examples separately
// if this example code seems daunting.
//
//
// The basic setup here is that for each frame, we generate a new array of
// 'noise' data, and then map it onto the LED matrix through a color palette.
//
// Periodically, the color palette is changed, and new noise-generation parameters
// are chosen at the same time.  In this example, specific noise-generation
// values have been selected to match the given color palettes; some are faster,
// or slower, or larger, or smaller than others, but there's no reason these
// parameters can't be freely mixed-and-matched.
//
// In addition, this example includes some fast automatic 'data smoothing' at
// lower noise speeds to help produce smoother animations in those cases.
//
// The FastLED built-in color palettes (Forest, Clouds, Lava, Ocean, Party) are
// used, as well as some 'hand-defined' ones, and some proceedurally generated
// palettes.

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define MAX_DIMENSION ((kMatrixWidth > kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

// The leds
CRGB *leds;

// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
uint16_t speed = 20; // speed is set dynamically once we've started up

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint16_t scale = 30; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
uint8_t **noise;

CRGBPalette16 currentPalette(halloween_gp);
uint8_t colorLoop = 1;

void NoisePlusPalette::setup()
{
  Pattern::setup();

  leds = new CRGB[NUM_LEDS];
  noise = new uint8_t*[MAX_DIMENSION];

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
  FastLED.setBrightness(BRIGHTNESS);

  random16_set_seed(0);

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();

  set(Event(
      "NoisePlusPalette_Loop", [](ECBArg a)
      { plebLoop(); },
      EVENT_TYPE_TEMP, 15));
}

void NoisePlusPalette::preStateChange(int s) {
  delete[] leds;
  delete[] noise;
}

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void NoisePlusPalette::fillnoise8()
{
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if (speed < 50)
  {
    dataSmoothing = 200 - (speed * 4);
  }

  for (int i = 0; i < MAX_DIMENSION; i++)
  {
    int ioffset = scale * i;
    for (int j = 0; j < MAX_DIMENSION; j++)
    {
      int joffset = scale * j;

      uint8_t data = inoise8(x + ioffset, y + joffset, z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing)
      {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i][j] = data;
    }
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;
}

void NoisePlusPalette::mapNoiseToLEDsUsingPalette()
{
  static uint8_t ihue = 0;

  for (int i = 0; i < kMatrixWidth; i++)
  {
    for (int j = 0; j < kMatrixHeight; j++)
    {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri = noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if (colorLoop)
      {
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if (bri > 127)
      {
        bri = 255;
      }
      else
      {
        bri = dim8_raw(bri * 2);
      }

      CRGB color = ColorFromPalette(currentPalette, index, bri);
      leds[XY(i, j)] = color;
    }
  }

  ihue += 1;
}

void NoisePlusPalette::plebLoop()
{
  // Periodically choose a new palette, speed, and scale
  ChangePaletteAndSettingsPeriodically();

  // generate noise data
  fillnoise8();

  // convert the noise data to colors in the LED array
  // using the current palette
  mapNoiseToLEDsUsingPalette();

  FastLED.show();
  // delay(10);
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.

// 1 = 5 sec per palette
// 2 = 10 sec per palette
// etc
#define HOLD_PALETTES_X_TIMES_AS_LONG 1

void NoisePlusPalette::ChangePaletteAndSettingsPeriodically()
{
  uint8_t secondHand = ((getCurStateEnt()->getElapsedMs() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;
    switch (secondHand)
    {
    case 0:
    case 15:
    case 30:
    case 45:
      // currentPalette = LavaColors_p;
      setupShawHalloweenPalette();
      Serial.println("setupShawHalloweenPalette()");
      speed = 20;
      scale = 50;
      colorLoop = 0;
      break;
    case 5:
    case 20:
    case 35:
    case 50:
      currentPalette = hopegoddess_gp;
      Serial.println("hopegoddess_gp");
      speed = 33;
      scale = 30;
      colorLoop = 1;
      break;
    case 10:
    case 25:
    case 40:
    case 55:
      currentPalette = halloween_gp;
      Serial.println("halloween_gp");
      speed = 28;
      scale = 30;
      colorLoop = 1;
      break;
    }
    /*
    if (secondHand == 0)
    {
      currentPalette = RainbowColors_p;
      Serial.println("RainbowColors_p");
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 5)
    {
      SetupPurpleAndGreenPalette();
      Serial.println("setupPurpleAndGreenPalette()");
      speed = 10;
      scale = 50;
      colorLoop = 1;
    }
    if (secondHand == 10)
    {
      SetupBlackAndWhiteStripedPalette();
      Serial.println("SetupBlackAndWhiteStripedPalette()");
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 15)
    {
      currentPalette = ForestColors_p;
      Serial.println("ForestColors_p");
      speed = 8;
      scale = 120;
      colorLoop = 0;
    }
    if (secondHand == 20)
    {
      currentPalette = CloudColors_p;
      Serial.println("CloudColors_p");
      speed = 4;
      scale = 30;
      colorLoop = 0;
    }
    if (secondHand == 25)
    {
      currentPalette = LavaColors_p;
      Serial.println("LavaColors_p");
      speed = 8;
      scale = 50;
      colorLoop = 0;
    }
    if (secondHand == 30)
    {
      currentPalette = OceanColors_p;
      Serial.println("OceanColors_p");
      speed = 20;
      scale = 90;
      colorLoop = 0;
    }
    if (secondHand == 35)
    {
      currentPalette = PartyColors_p;
      Serial.println("PartyColors_p");
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 40)
    {
      SetupRandomPalette();
      Serial.println("SetupRandomPalette()");
      speed = 20;
      scale = 20;
      colorLoop = 1;
    }
    if (secondHand == 45)
    {
      SetupRandomPalette();
      Serial.println("SetupRandomPalette()");
      speed = 50;
      scale = 50;
      colorLoop = 1;
    }
    if (secondHand == 50)
    {
      SetupRandomPalette();
      Serial.println("SetupRandomPalette()");
      speed = 90;
      scale = 90;
      colorLoop = 1;
    }
    if (secondHand == 55)
    {
      currentPalette = RainbowStripeColors_p;
      Serial.println("RainbowStripeColors_p");
      speed = 30;
      scale = 20;
      colorLoop = 1;
    }
    */
  }
}

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is
// a bright hue, the third is a bright pastel, and the last is
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
void NoisePlusPalette::SetupRandomPalette()
{
  currentPalette = CRGBPalette16(
      CHSV(random8(), 255, 32),
      CHSV(random8(), 255, 255),
      CHSV(random8(), 128, 255),
      CHSV(random8(), 255, 255));
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void NoisePlusPalette::SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void NoisePlusPalette::SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(
      green, green, black, black,
      purple, purple, black, black,
      green, green, black, black,
      purple, purple, black, black);
}

void NoisePlusPalette::setupShawHalloweenPalette()
{
  CHSV orange = CHSV(10, 255, 225);
  CHSV purple = CHSV(200, 250, 150); // 200, 150
  CHSV green = CHSV(70, 250, 150);   // 75, 200

  currentPalette = CHSVPalette16(
      purple, purple, purple, purple,
      purple, purple, purple, orange,
      orange, orange, orange, orange,
      orange, orange, green, green);
}

//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//
uint16_t NoisePlusPalette::XY(uint8_t x, uint8_t y)
{
  uint16_t i;
  if (kMatrixSerpentineLayout == false)
  {
    i = (y * kMatrixWidth) + x;
  }
  if (kMatrixSerpentineLayout == true)
  {
    if (y & 0x01)
    {
      // Odd rows run backwards
      int reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    }
    else
    {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}
