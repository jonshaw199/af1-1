import { WebSocket as WS } from "ws";
import { Types } from "mongoose";

import MessageBroker from "../messageBroker";
import { WebSocketClient, Request } from "../types";
import deviceModel from "../models/device";

export async function handleWS(w: WS, req: Request, next) {
  const ws = w as WebSocketClient;
  ws.path = req.path;
  try {
    ws.deviceId = new Types.ObjectId(req.query.deviceId?.toString());
    const device = await deviceModel.findById(ws.deviceId);
    ws.orgId = device.orgId;
  } catch (e) {
    next(e);
  }

  ws.on("message", (m) => {
    process.stdout.write("<");
    try {
      const packet = JSON.parse(m.toString());
      MessageBroker.handlePacket({ packet, orgId: ws.orgId });
    } catch (e) {
      console.log(`Unable to parse JSON: ${m} (${e})`);
    }
  });

  ws.on("error", (err) => {
    console.log(`${ws.path} error: ` + err);
  });

  ws.on("close", () => {
    console.log(`Closing connection at ${ws.path}`);
  });

  next();
}
