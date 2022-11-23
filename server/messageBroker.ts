import { Instance } from "express-ws";
import { WebSocket as WS } from "ws";
import { Types } from "mongoose";

import {
  MessageType,
  BroadcastMessage,
  TopicMessage,
  WebSocket,
  Request,
} from "./types";
import MQTT, { SubscriberId } from "./mqtt";
import messageModel from "./models/message";
import deviceModel from "./models/device";

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

  public static getSubscriberIDs(topic: string) {
    return MQTT.getSubscriberIDs(topic);
  }

  public static clearTopicTree() {
    MQTT.clearTopicTree();
  }

  public static getSubscribers({
    topic,
    orgId,
  }: {
    topic?: string;
    orgId?: Types.ObjectId;
  }) {
    const subscribers = this.getSubscriberIDs(topic);
    return Array.from(this.expressWsInstance.getWss().clients).filter(
      (w: WebSocket) =>
        (!topic || subscribers.has(w.deviceId)) && (!orgId || w.orgId == orgId)
    );
  }

  private static broadcast({
    orgId,
    msg,
  }: {
    orgId?: Types.ObjectId;
    msg: BroadcastMessage<any>;
  }) {
    this.getSubscribers({ topic: msg.topic, orgId }).forEach((subscriber) =>
      subscriber.send(msg)
    );
  }

  public static handleMQTTMsg({
    orgId,
    msg,
  }: {
    orgId?: Types.ObjectId;
    msg: TopicMessage;
  }) {
    messageModel.create(
      {
        senderId: msg.senderId,
        state: msg.state,
        type: msg.type,
      },
      (err, m) => {
        if (err) {
          console.log(`Error creating message: ${err}`);
        }
      }
    );

    switch (msg.type) {
      case MessageType.TYPE_MQTT_SUBSCRIBE:
        const subscribeMsg = msg as TopicMessage;
        console.log(
          `Subscribe device ID: ${msg.senderId}; topic: ${subscribeMsg.topic}`
        );
        this.subscribe(msg.senderId, subscribeMsg.topic);
        break;
      case MessageType.TYPE_MQTT_UNSUBSCRIBE:
        const unsubscribeMsg = msg as TopicMessage;
        console.log(
          `Unsubscribe device ID: ${msg.senderId}; topic: ${unsubscribeMsg.topic}`
        );
        this.unsubscribe(msg.senderId, unsubscribeMsg.topic);
        break;
      case MessageType.TYPE_MQTT_BROADCAST:
        const broadcastMsg = msg as BroadcastMessage<any>;
        console.log(`Broadcast for topic ${broadcastMsg.topic}`);
        this.broadcast({
          orgId,
          msg: broadcastMsg,
        });
        break;
    }
  }
}
