import { v4 as uuidv4 } from 'uuid';
import express from 'express';
import mongoose from 'mongoose';
import dotenv from "dotenv-defaults";
import http from 'http';
import WebSocket from 'ws';

import mongo from './mongo';
import wsConnect from './wsConnect';

mongo.connect();

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({server});
const db = mongoose.connection;

db.once('open', () => {
    console.log("MongoDB connected!");
    
    wss.on('connection', (ws) => {
        //wsConnect.initData(ws);
        ws.id = uuidv4();
        ws.box = ''; // current active ChatBox name (for ws client)
        ws.onmessage = wsConnect.onMessage(wss, ws);
    });
});

const PORT = process.env.PORT || 4000;
server.listen(PORT, () => {
    console.log(`Listening on http://localhost:${PORT}!`);
});