import { Instance } from "express-ws";
import { WebSocket } from "./types";

import MQTT, { SubscriberId } from "./mqtt";
import { Types } from "mongoose";

export default class MessageBroker {
  private static expressWsInstance: Instance;

  public static init(i: Instance) {
    this.expressWsInstance = i;
  }

  public static subscribe(subscriber: SubscriberId, topic: string) {
    MQTT.subscribe(subscriber, topic);
  }

  public static unsubscribe(subscriber: SubscriberId, topic: string) {
    MQTT.unsubscribe(subscriber, topic);
  }

  public static getSubscribers(topic: string) {
    return MQTT.getSubscribers(topic);
  }

  public static clearTopicTree() {
    MQTT.clearTopicTree();
  }

  public static getClients(orgId: Types.ObjectId, topic: string) {
    const subscribers = this.getSubscribers(topic);
    return Array.from(this.expressWsInstance.getWss().clients).filter(
      (w: WebSocket) => w.orgId == orgId && subscribers.has(w.deviceId)
    );
  }
}
