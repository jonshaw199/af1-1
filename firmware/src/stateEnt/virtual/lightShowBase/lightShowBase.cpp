#include "lightShowBase.h"
#include "state.h"

#ifdef ARDUINO_M5Stick_C
#include <M5StickCPlus.h>
#undef min // https://github.com/m5stack/M5Stack/issues/97
#include "state.h"
#include "img/mountains.h"
#include "stateEnt/virtual/lightShowBase/lightShowBase.h"

static int lastState = INITIAL_STATE;
#endif

#ifdef VS1053_CS_PIN

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

/*
  -DVS1053_SCLK_PIN=18
  -DVS1053_MISO_PIN=19
  -DVS1053_MOSI_PIN=23
  -DVS1053_CS_PIN=5
  -DVS1053_RST_PIN=22
  -DVS1053_XDCS_PIN=21
  -DVS1053_SDCS_PIN=17
  -DVS1053_DREQ_PIN=16
*/

// define the pins used
// #define CLK VS1053_SCLK_PIN  // (18) // SPI Clock, shared with SD card
// #define MISO VS1053_MISO_PIN // (19) // Input data, from VS1053/SD card
// #define MOSI VS1053_MOSI_PIN // (23) // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins.
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET VS1053_RST_PIN // (22) // VS1053 reset pin (output)
#define BREAKOUT_CS VS1053_CS_PIN     // (5)  // VS1053 chip select pin (output)
#define BREAKOUT_DCS VS1053_XDCS_PIN  // (21) // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
// #define SHIELD_RESET -1 // VS1053 reset pin (unused!)
// #define SHIELD_CS 7     // VS1053 chip select pin (output)
// #define SHIELD_DCS 6    // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS VS1053_SDCS_PIN // (17) // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ VS1053_DREQ_PIN // (16) // VS1053 Data request, ideally an Interrupt pin

static Adafruit_VS1053_FilePlayer musicPlayer =
    // create breakout-example object!
    // Baby dont do it!
    // Adafruit_VS1053_FilePlayer(MOSI, MISO, CLK, BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
    Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

/// File listing helper
void printDirectory(File dir, int numTabs)
{
  while (true)
  {

    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      // Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++)
    {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory())
    {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    }
    else
    {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void LightShowBase::pausePlaying(bool p)
{
  Serial.print("Audio pause: ");
  Serial.println(p);
  musicPlayer.pausePlaying(p);
}

void LightShowBase::stopPlaying()
{
  Serial.println("Audio stop");
  musicPlayer.stopPlaying();
}

bool LightShowBase::playFile(String f)
{
  Serial.print(F("Playing file "));
  Serial.println(f);
  return musicPlayer.playFullFile(f.c_str());
}

#endif

static bool motion = false;

void LightShowBase::setup()
{
  Base::setup();

#ifdef ARDUINO_M5Stick_C
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setRotation(0);
  M5.Lcd.pushImage(0, 0, MOUNTAINS_WIDTH, MOUNTAINS_HEIGHT, (uint16_t *)mountains);
#endif

#ifdef IR_SENSOR_PIN
  pinMode(IR_SENSOR_PIN, INPUT); // sensor pin INPUT
#endif

#ifdef VS1053_CS_PIN
  if (!musicPlayer.begin())
  { // initialise the music player
    Serial.println(F("CRITICAL: Couldn't find VS1053, do you have the right pins defined?"));
    return;
    // while (1)
    // ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS))
  {
    Serial.println(F("CRITICAL: SD failed, or not present"));
    return;
    // while (1)
    // ; // don't do anything more
  }

  // list files
  printDirectory(SD.open("/"), 0);

  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(20, 20);

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  // musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background
  // audio playing
  // musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT); // DREQ int

  // Play one file, don't return until complete
  // Serial.println(F("Playing track 001"));
  // musicPlayer.playFullFile("/track001.mp3");
  // Play another file in the background, REQUIRES interrupts!
  // Serial.println(F("Playing track 002"));
  // musicPlayer.startPlayingFile("/track002.mp3");
  // musicPlayer.startPlayingFile("/track001.mp3");
#endif

#define AUTO_SHUTOFF_MIN (24 * 60 /* EOD */ + 7 * 60 /* TZ Offset */)
  if (timeClient.isTimeSet())
  {
    unsigned long curSec = timeClient.getEpochTime();
    unsigned long beginDaySec = curSec - timeClient.getHours() * 60 * 60 - timeClient.getMinutes() * 60 - timeClient.getSeconds();
    unsigned long autoShutoffSec = beginDaySec + AUTO_SHUTOFF_MIN * 60;
    Serial.print("Auto shutoff seconds: ");
    Serial.println(autoShutoffSec);
    set(Event(
        "LightShowBase_AutoShutoff",
        [](ECBArg a)
        {
          setRequestedState(STATE_HOME);
        },
        EVENT_TYPE_TEMP, 0, 1, autoShutoffSec, START_EPOCH_SEC));
  }
}

void LightShowBase::loop()
{
  Base::loop();

#ifdef ARDUINO_M5Stick_C
  M5.update(); // Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.wasReleased())
  { // If the button A is pressed.  如果按键 A 被按下
    M5.Lcd.print('A');
    AF1JsonDoc body;
    body["type"] = TYPE_CHANGE_STATE;
    lastState = lastState == STATE_HOME ? STATE_PATTERN_NOISE : STATE_HOME;
    body["state"] = lastState;
    httpPost(REMOTE_URL, body);
  }
  /*else if (M5.BtnB.wasReleased())
  { // If the button B is pressed. 如果按键
    // B 被按下，
    // M5.Lcd.print('B');
    AF1::setRequestedState(STATE_RC2);
  }*/
  /*else if (M5.BtnB.wasReleasefor(
               700))
  { // The button B is pressed for 700ms. 按键 B 按下
    // 700ms,屏幕清空
    M5.Lcd.fillScreen(
        BLACK); // Set BLACK to the background color.  将黑色设置为底色
    M5.Lcd.setCursor(0, 0);
  }*/
#endif

#ifdef IR_SENSOR_PIN
  if (digitalRead(IR_SENSOR_PIN) == 1)
  {
    // Motion
    if (!motion)
    {
      motion = true;
      Serial.println("Local IR sensor motion begin");
      AF1Msg msg(TYPE_MOTION);
      msg.json()["motion"] = true;
      msg.setRecipients({255});
      pushOutbox(msg);
    }
  }
  else
  {
    if (motion)
    {
      motion = false;
      Serial.println("Local IR sensor motion end");
      AF1Msg msg(TYPE_MOTION);
      msg.json()["motion"] = false;
      msg.setRecipients({255});
      pushOutbox(msg);
    }
  }
#endif
}

bool LightShowBase::doScanForPeersESPNow()
{
  return false;
}

void LightShowBase::onConnectWSServer()
{
  sendMsgInfo({255});
}

AF1JsonDoc LightShowBase::getInfo()
{
  AF1JsonDoc i = Base::getInfo();
#ifdef ARDUINO_M5Stick_C
  i["arduinoM5StickC"] = true;
#endif
#ifdef VS1053_CS_PIN
  i["vs1053"] = true;
#endif
#ifdef IR_SENSOR_PIN
  i["ir"] = true;
#endif
#ifdef JS_IP_A
  i["staticIp"] = String(JS_IP_A) + "." + String(JS_IP_B) + "." + String(JS_IP_C) + "." + String(JS_IP_D);
#endif
#ifdef MASTER
  i["master"] = true;
#endif
  return i;
}

msg_handler LightShowBase::getInboxHandler()
{
  return [](AF1Msg m)
  {
    Base::handleInboxMsg(m);

    switch (m.getType())
    {
    case TYPE_MOTION:
      Serial.print("Notification msg received: motion ");
      if (m.json()["motion"] == true)
      {
        Serial.println(" begin...");
#ifdef VS1053_CS_PIN
        playFile("/track001.mp3");
#endif
      }
      else
      {
        Serial.println(" end.");
      }
      break;
    }
  };
}
