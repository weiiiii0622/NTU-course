// --------------------------------------------------------------------------
// OSC via WebSocket
// by Contra
// --------------------------------------------------------------------------
"use strict";

const webSocket = require("ws");

const osc = require("osc");
const maxAPI = require("max-api");

const wss = new webSocket.Server({ port: 8081 });

let webSocketPort;

wss.on("connection", function connection(ws) {
	console.log("connection");

	let isConnected = true;

	webSocketPort = new osc.WebSocketPort({
		socket: ws
	});

	ws.on("message", function incoming(message) {
		console.log("received : ", message);

		//message data type is ArrayBuffer
		const msgParsed = osc.readPacket(message, { metadata: true });
		console.log("received parsed : ", msgParsed);
		console.log("received parsed : ", msgParsed['args']);
		maxAPI.outlet('message', msgParsed);
		
		// Parse data with format [isActive, x-coord, y-coord, x-speed, y-speed]
		const isActive = msgParsed['args'][0]['value'];	// 0: notActive, 1: Active
		const x_coord = msgParsed['args'][1]['value'];	// Float
		const y_coord = msgParsed['args'][2]['value'];	// Float
		const x_speed = msgParsed['args'][3]['value'];	// Float
		const y_speed = msgParsed['args'][4]['value'];	// Float
		
		console.log("Packed: ", [isActive, x_coord, y_coord, x_speed, y_speed]);
		
		if(msgParsed['address'] == '/bird'){
			console.log('Bird!');
			maxAPI.outlet('bird', [isActive, x_coord, y_coord, x_speed, y_speed]);
		}
		if(msgParsed['address'] == '/wood'){
			console.log('Wood!');
			maxAPI.outlet('wood', [isActive, x_coord, y_coord, x_speed, y_speed]);
		}
		if(msgParsed['address'] == '/pig'){
			console.log('pig!');
			maxAPI.outlet('pig', [isActive, x_coord, y_coord, x_speed, y_speed]);
		}
		if(msgParsed['address'] == '/slingshot'){
			console.log('slingshot!');
			maxAPI.outlet('slingshot', [isActive, x_coord, y_coord, x_speed, y_speed]);
		}
	});

	ws.on("error", (err) => {
		console.log("error:", err);
	})

	ws.on("close", function stop() {
		maxAPI.removeHandlers("send");
		console.log("Connection closed");

		ws.terminate();

		isConnected = false;
	});

	// Handle the Max interactions here...
	maxAPI.addHandler("send", (...args) => {
		//console.log("send args: " + args);
		if (webSocketPort && isConnected) {
			webSocketPort.send({
				address: "/max/midi",
				args: [
					{
						type: "i",
						value: args[0]
					}
				]
			});
		}

	});
});

maxAPI.addHandler(maxAPI.MESSAGE_TYPES.ALL, (handled, ...args) => {
	if (!handled) {
		// Max.post('No client connected.')
		// just consume the message
	}
});
