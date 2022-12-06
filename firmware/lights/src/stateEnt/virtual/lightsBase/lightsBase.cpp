#include "lightsBase.h"
#include "state.h"

#ifdef ARDUINO_M5Stick_C
#include <M5StickCPlus.h>
#undef min // https://github.com/m5stack/M5Stack/issues/97

#include "img/mountains.h"
#endif

#define MAX_PACKET_ID 5 // Can't store many unacked

static uint8_t nextPacketId;
static std::map<uint8_t, AF1Msg> unackedPackets;

CRGB *LightsBase::leds;

CRGBPalette16 LightsBase::currentPalette;
CRGBPalette16 LightsBase::targetPalette;
TBlendType LightsBase::currentBlending = LINEARBLEND;
uint8_t LightsBase::currentBrightness = 200;
uint8_t LightsBase::currentPaletteIndex;
// Add or remove palette names from this list to control which color
// palettes are used, and in what order.
const TProgmemRGBPalette16 *LightsBase::activePaletteList[] = {
    &RetroC9_p,
    &BlueWhite_p,
    &RedGreenWhite_p,
    &Snow_p,
    &RedWhite_p,
    &Ice_p,
    &Holly_p}; // to do
std::vector<uint8_t> LightsBase::sceneStates;
uint8_t LightsBase::currentSceneIndex;

void LightsBase::init()
{
  leds = new CRGB[CNT];

#if CNT
#if CNT_A
  FastLED.addLeds<LED_TYPE_A, LED_PIN_A, LED_ORDER_A>(leds, CNT);
#endif
#if CNT_B
  FastLED.addLeds<LED_TYPE_B, LED_PIN_B, LED_ORDER_B>(leds, CNT);
#endif
#endif
}

void LightsBase::setup()
{
  Base::setup();

#ifdef ARDUINO_M5Stick_C
  M5.Lcd.fillScreen(TFT_WHITE);
  M5.Lcd.setRotation(0);
  M5.Lcd.pushImage(0, 0, MOUNTAINS_WIDTH, MOUNTAINS_HEIGHT, (uint16_t *)mountains);
#endif
}

void LightsBase::loop()
{
  Base::loop();

#ifdef ARDUINO_M5Stick_C
  M5.update(); // Read the press state of the key.  读取按键 A, B, C 的状态
  if (M5.BtnA.wasReleased())
  { // If the button A is pressed.  如果按键 A 被按下
    M5.Lcd.print('A');
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
}

bool LightsBase::doScanForPeersESPNow()
{
  return false;
}

void LightsBase::onConnectWSServer()
{
  AF1Msg m(TYPE_MQTT_SUBSCRIBE);
  m.json()["topic"] = LIGHTS_ROUTE;
  m.json()["qos"] = 0;
  pushOutbox(m);
}

void LightsBase::onConnectWSServerFailed()
{
  Serial.println("Restarting due to failed WS connection");
  ESP.restart();
}

void LightsBase::onConnectWifiFailed()
{
  Serial.println("Restarting due to failed wifi connection");
  ESP.restart();
}

void LightsBase::handleInboxMsg(AF1Msg &m)
{
  Base::handleInboxMsg(m);
  switch (m.getType())
  {
  case TYPE_MQTT_PUBLISH:
  {
    String topic = m.json()["topic"];
    if (topic == "/lights/state")
    {
      Serial.println("Pattern msg");
      setRequestedState(m.getState());
    }
    else if (topic == "/lights/color")
    {
      Serial.println("Color msg");
    }

    // Acks
    uint8_t q = m.json()["qos"];
    if (q == 1)
    {
      int p = m.json()["packetId"];
      AF1Msg res(TYPE_MQTT_PUBACK);
      res.json()["packetId"] = p;
      pushOutbox(res);
    }
    else if (q == 2)
    {
      int p = m.json()["packetId"];
      AF1Msg res(TYPE_MQTT_PUBREC);
      res.json()["packetId"] = p;
      pushOutbox(res);
    }
  }
  break;
  case TYPE_MQTT_PUBACK:
  case TYPE_MQTT_PUBCOMP:
  {
    uint8_t p = m.json()["packetId"];
    unackedPackets.erase(p);
  }
  break;

  case TYPE_MQTT_PUBREC:
  {
    uint8_t p = m.json()["packetId"];
    unackedPackets[p] = m;
    AF1Msg res(TYPE_MQTT_PUBREL);
    res.json()["packetId"] = p;
    pushOutbox(res);
  }
  break;
  case TYPE_MQTT_PUBREL:
  {
    int p = m.json()["packetId"];
    AF1Msg res(TYPE_MQTT_PUBCOMP);
    res.json()["packetId"] = p;
    pushOutbox(res);
  }
  break;
  }
}

msg_handler LightsBase::getInboxHandler()
{
  return handleInboxMsg;
}

msg_handler LightsBase::getOutboxHandler()
{
  return [](AF1Msg &m)
  {
    uint8_t q = m.json()["qos"];
    switch (m.getType())
    {
    case TYPE_MQTT_PUBLISH:
    case TYPE_MQTT_PUBREL:
      if (q)
      {
        m.json()["packetId"] = nextPacketId;
        unackedPackets[nextPacketId] = m;
        nextPacketId = (nextPacketId + 1) % MAX_PACKET_ID;
      }
      break;
    }
    Base::handleOutboxMsg(m);
  };
}

void LightsBase::advanceTargetPalette()
{
  const uint8_t numberOfPalettes = sizeof(activePaletteList) / sizeof(activePaletteList[0]);
  currentPaletteIndex = addmod8(currentPaletteIndex, 1, numberOfPalettes);
  targetPalette = *(activePaletteList[currentPaletteIndex]);
  Serial.print("Changing palette: ");
  Serial.println(currentPaletteIndex);
}
