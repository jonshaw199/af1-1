import { WebSocket as WS } from "ws";
import { Request as Req } from "express";
import { Document, Types } from "mongoose";

export type Nullable<T> = null | undefined | T;

export enum MessageType {
  TYPE_MOTION = 0,
  TYPE_NONE = 100,
  TYPE_HANDSHAKE_REQUEST,
  TYPE_HANDSHAKE_RESPONSE,
  TYPE_CHANGE_STATE,
  TYPE_RUN_DATA,
  TYPE_RC_DATA,
  TYPE_TIME_SYNC,
  TYPE_TIME_SYNC_RESPONSE,
  TYPE_TIME_SYNC_START,
  TYPE_INFO,
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

export type Info = {
  arduinoM5StickC?: boolean;
  vs1053?: boolean;
  webClient?: boolean;
  ir?: boolean;
  staticIp?: string;
  master?: boolean;
  esp32?: boolean;
};

export type Message = {
  state?: State | number;
  type: MessageType | number;
  senderID: Types.ObjectId;
  _id: Types.ObjectId;
};

export type InfoMessage = Message & {
  info: Info;
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
};

// API

export type AuthRequest = {
  email: string;
  password: string;
};

export type AuthResponse = {
  success: boolean;
  msg: string;
  token: string;
  user: User;
};

export type UserResponse = {
  msg: string;
  success: boolean;
  user: User;
};

export type CreateUserResponse = UserResponse & {
  token: string;
};

export type CreateOrgResponse = {
  success: boolean;
  msg: string;
  org: Org;
};
