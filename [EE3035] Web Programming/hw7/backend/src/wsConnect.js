// Websocket Connect
import ChatBoxModel from "./models/chatbox";
import MessageModel from "./models/Message";
import UserModel from './models/User';
import { ObjectId } from "mongoose";

const chatBoxes = {}; // {chatBoxName, Set<ws>}

// const broadcastMessage = (ws, data, status) => {
//     ws.forEach((client) => {
//         sendData(data, client);
//         sendStatus(status, client);
//     });
// };

const makeName = (name, to) => {
    return [name, to].sort().join('_');
};

const validateUser = async (name) => {
    let user = await UserModel.findOne({ name });
    //console.log(user);
    if(user) return user;
    else{
        return user = await new UserModel({ name }).save();
    }
}

const validateChatBox = async (name, participants) => {
    let box = await ChatBoxModel.findOne({ name });
    if (!box){
        box = await new ChatBoxModel({ name, users: participants }).save(); // users:[sender, receiver];
    }
    return box.populate(["users", {path: 'messages', populate: 'sender' }]);
    //return box.populate('messages');
};

const sendData = async (data, ws) => {
    console.log(JSON.stringify(data));
    ws.send(JSON.stringify(data));
};
const sendStatus = (payload, ws) => {
    sendData(["status", payload], ws);
};

export default {
    // initData: (ws) => {
    //      Message.find().sort({ created_at: -1 }).limit(100).exec((err, res) => {
    //         if(err) throw err;
    //         //initialize app with existing messages
    //         sendData(["init", res], ws);
    //     });
    // },

    onMessage: (wss, ws) => (
        async (byteString) => {
            
            const { data } = byteString
            const {task, payload} = JSON.parse(data);
            //console.log("task: "+task+" payload:"+payload);
            switch (task) {
                case 'CHAT':{
                    const { name, to } = payload;
                    let chatBoxName = makeName(name, to);
                    let sender = await validateUser(name);
                    let receiver = await validateUser(to);
                    let chatBox = await validateChatBox(chatBoxName, [sender._id, receiver._id]);
                    
                    // Handle Open ChatBoxes
                    if(!chatBoxes[chatBoxName]){
                        chatBoxes[chatBoxName] = new Set();
                    }
                    if (ws.box !== "" && chatBoxes[ws.box]){
                        chatBoxes[ws.box].delete(ws);
                    }
                    ws.box = chatBoxName;
                    chatBoxes[ws.box].add(ws);

                    // Achieve messages
                    let msgs = [];
                    chatBox.messages.map((message) => {
                        msgs.push({name:message.sender.name, body:message.body});
                    })
                    // Send to WS CLient
                    sendData(["init", msgs], ws);
                    sendStatus({type:"success", msg:"Init Success."}, ws);
                    
                    // MessageModel.find({$or:[{name: name},{name: to}]}).sort({created_at: -1}).limit(100).exec((err, res) => {
                    //     if(err) throw err;
                    //     sendData(["init", res], ws);
                    // })
                    // broadcastMessage(
                    //     chatBoxes[ws.box], 
                    //     ['output', [messages]], 
                    //     {type: "success", msg: "Message sent."}
                    // );

                    break;
                }
                case 'MESSAGE':{
                    const { name, to, body } = payload;
                    let user = await UserModel.findOne({ name });
                    let chatBox = await ChatBoxModel.findOne({name:makeName(name, to)});
                    let msg = await new MessageModel({ chatBox:chatBox._id, sender:user._id, body:body }).save();
                    chatBox.messages.push(msg._id);
                    chatBox.save();

                    // Send to WS CLient
                    chatBoxes[ws.box].forEach((client) => {
                        sendData(["output", [{name, body}]], client);
                    });
                    sendStatus({type:"success", msg:"Message sent."}, ws);
                    break;
                }
                case 'CLEAR':{
                    
                    break;
                }
                case 'CLOSE':{
                    if (ws.box !== "" && chatBoxes[ws.box])
                        // user(ws) was in another chatbox
                        chatBoxes[ws.box].delete(ws);
                    break;
                }


                // case 'input': {
                //     const { name, body } = payload
                //     // Save payload to DB
                //     const message = new Message({ name, body })
                //     try { 
                //         await message.save();
                //     } catch (e) { 
                //         throw new Error("Message DB save error: " + e); 
                //     }
                //     broadcastMessage(
                //         wss, 
                //         ['output', [payload]], 
                //         {type: "success", msg: "Message sent."}
                //     );
                //     break;
                // }
                // case 'clear': {
                //     Message.deleteMany({}, () => {
                //         broadcastMessage(
                //             wss, 
                //             ["cleared"], 
                //             {type: "info", msg: "Message cache cleared."}
                //         );
                //     });
                //     break;
                // }
                default: break;
            }
        }
    )
}