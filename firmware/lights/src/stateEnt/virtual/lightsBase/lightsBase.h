#ifndef STATEENT_VIRTUAL_LIGHTSBASE_LIGHTSBASE_H_
#define STATEENT_VIRTUAL_LIGHTSBASE_LIGHTSBASE_H_

#include <AF1.h>

class LightsBase : public Base
{
protected:
  static void handleInboxMsg(AF1Msg &m);

public:
  static void init();
  void setup();
  void loop();
  bool doScanForPeersESPNow();
  void onConnectWSServer();
  void onConnectWifiFailed();
  msg_handler getInboxHandler();
};

#endif
