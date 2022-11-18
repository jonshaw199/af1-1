import mongoose from "mongoose";

import { Message } from "../types";

const messageSchema = new mongoose.Schema<Message>(
  {
    state: { type: Number },
    senderID: { type: mongoose.Schema.Types.ObjectId, required: true },
    type: { type: Number, required: true },
  },
  { capped: { size: 1024 } }
);

const messageModel = mongoose.model<Message>("Message", messageSchema);

export default messageModel;
