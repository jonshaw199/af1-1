import { WebSocket as WS } from "ws";
import { Request as Req } from "express";
import { Document, Types } from "mongoose";

export type Nullable<T> = null | undefined | T;

export enum MessageType {
  TYPE_MQTT_SUBSCRIBE = 0,
  TYPE_MQTT_UNSUBSCRIBE,
  TYPE_MQTT_PAYLOAD,
  TYPE_NONE = 100,
  TYPE_HANDSHAKE_REQUEST,
  TYPE_HANDSHAKE_RESPONSE,
  TYPE_CHANGE_STATE,
  TYPE_TIME_SYNC,
  TYPE_TIME_SYNC_RESPONSE,
  TYPE_TIME_SYNC_START,
}

export enum State {
  STATE_HOME = 0,
  STATE_PATTERN_TWINKLEFOX,
  STATE_INIT = 100,
  STATE_PURG,
  STATE_OTA,
  STATE_RESTART,
  STATE_IDLE_BASE,
  STATE_SYNC_TEST,
}

export enum Board {
  BOARD_WEMOS_D1_MINI32 = "wemos_d1_mini32",
  BOARD_HELTEC_WIFI_KIT_32 = "heltec_wifi_kit_32",
  BOARD_ESP32DEV = "esp32dev",
  BOARD_M5STICK_C = "m5stick-c",
  BOARD_ESP_WROVER_KIT = "esp-wrover-kit",
  BOARD_OTHER = "BOARD_OTHER",
}

export type Message = {
  state?: State | number;
  type: MessageType | number;
  senderID: Types.ObjectId;
  _id: Types.ObjectId;
};

export type TopicMessage = Message & {
  topic: string;
};

export type PayloadMessage<T> = TopicMessage & {
  data: T;
};

export type WebSocket = WS & {
  path: Nullable<string>;
  deviceId: Nullable<Types.ObjectId>;
  orgId: Nullable<Types.ObjectId>;
};

export type Request = Req & {
  user: Nullable<Document<User>>;
  token: Nullable<string>;
};

export type User = {
  name: string;
  email: string;
  password: string;
  _id: Types.ObjectId;
  orgId: Types.ObjectId;
};

export type Org = {
  _id: Types.ObjectId;
  name: string;
};

export type Device = {
  _id: Types.ObjectId;
  orgId: Types.ObjectId;
  board: Board;
  name: string;
};

// API

export type AuthRequest = {
  email: string;
  password: string;
};

export type AuthResponse = {
  msg: string;
  token: string;
  user: User;
};

export type UserResponse = {
  msg: string;
  user: User;
};

export type CreateUserResponse = UserResponse & {
  token: string;
};

export type CreateOrgResponse = {
  msg: string;
  org: Org;
};

export type DeviceResponse = {
  msg: string;
  device: Device;
};

export type MessageResponse = {
  msg: string;
  message: Message;
};
