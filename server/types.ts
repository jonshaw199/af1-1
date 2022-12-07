import { WebSocket as WS } from "ws";
import { Request as Req } from "express";
import { Document, Types } from "mongoose";

export type Nullable<T> = null | undefined | T;

export enum MessageType {
  TYPE_MQTT_SUBSCRIBE = 0,
  TYPE_MQTT_SUBACK,
  TYPE_MQTT_UNSUBSCRIBE,
  TYPE_MQTT_UNSUBACK,
  TYPE_MQTT_PUBLISH,
  // QOS 1
  TYPE_MQTT_PUBACK,
  // QOS 2
  TYPE_MQTT_PUBREC,
  TYPE_MQTT_PUBREL,
  TYPE_MQTT_PUBCOMP,
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
  STATE_PATTERN_NOISE,
  STATE_PATTERN_PICKER,
  STATE_PATTERN_BEATWAVE,
  STATE_PATTERN_RIPPLE,
  STATE_PATTERN_EVERYOTHER,
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

export enum Topics {
  LIGHTS_STATE = "/lights/state",
  LIGHTS_COLOR = "/lights/color",
}

export enum Palette {
  // Christmas
  RetroC9_p,
  BlueWhite_p,
  RedGreenWhite_p,
  Snow_p,
  RedWhite_p,
  Ice_p,
  Holly_p,
  // Other
  RainbowColors_p,
  PartyColors_p,
}

export type SubscriberId = string;
export type PacketId = number;
export type QOS = number;
export type Topic = string;
export type Subscriber = {
  subscriberId: SubscriberId;
  topic: Topic;
  qos?: QOS;
};

export type Message = {
  type: MessageType | number;
  senderId: string;
  state?: State | number;
};

export type Packet = Message & {
  topic: Topic;
  qos?: QOS;
  packetId?: PacketId;
};

export type PacketLightsColor = Packet & {
  h?: Nullable<number>;
  s?: Nullable<number>;
  v?: Nullable<number>;
  palette?: Nullable<Palette>;
};

export type WebSocketClient = WS & {
  path: Nullable<string>;
  deviceId: Nullable<Types.ObjectId>;
  orgId: Nullable<Types.ObjectId>;
};

export type SubscriberClient = WebSocketClient & {
  topic: Topic;
  qos: QOS;
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

export type CreateUserRequest = Omit<User, "_id">;

export type UserResponse = {
  msg: string;
  user: User;
};

export type CreateUserResponse = UserResponse & {
  token: string;
};

export type UserListResponse = {
  users: User[];
};

export type CreateOrgRequest = Omit<Org, "_id">;

export type OrgResponse = {
  msg: string;
  org: Org;
};

export type OrgListResponse = {
  orgs: Org[];
};

export type CreateDeviceRequest = Omit<Device, "_id">;

export type DeviceResponse = {
  msg: string;
  device: Device;
};

export type DeviceListResponse = {
  devices: Device[];
};

export type MessageResponse = {
  msg: string;
  message: Message;
};
