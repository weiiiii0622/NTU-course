import { useState, useEffect } from "react";
import { message } from "antd";
import { createContext, useContext } from "react";

const client = new WebSocket('ws://localhost:4000');

const LOCALSTORAGE_KEY = "save-me";
const savedMe = localStorage.getItem(LOCALSTORAGE_KEY);

const ChatContext = createContext({
    status: {},
    me: "",
    hasInit: false,
    signedIn: false,
    messages: [],
    chatBoxes: [],
    startChat: () => {},
    sendMessage: () => {},
    clearMessages: () => {},
    closeWS: () => {},
});

const ChatProvider = (props) => {
    const [status, setStatus] = useState({});
    const [hasInit, setHasInit] = useState(false);
    const [me, setMe] = useState(savedMe || "");
    const [signedIn, setSignedIn] = useState(false);
    const [messages, setMessages] = useState([]);
    const [chatBoxes, setChatBoxes] = useState([]); // {label, children, key}

    useEffect(() => {
        if (signedIn) {
            localStorage.setItem(LOCALSTORAGE_KEY, me);
        }
    }, [me, signedIn]);
    
    client.onmessage = (byteString) => {
        const {data} = byteString;
        const [task, payload] = JSON.parse(data);
        switch(task){
            case 'init':{
                setMessages(payload);
                break;
            }
            case 'output':{
                setMessages(() => [...messages, ...payload]);
                break;
            }
            case 'status':{
                setStatus(payload);
                break;
            }
            case "cleared": {
                setMessages([]);
                break;
            }
            default: break;
        }
    }

    client.onclose = () => {
        sendData({
            task: 'CLOSE',
            payload: {},
        });
    }

    const startChat = async ({name, to}) => {
        //console.log("name: "+name+" to: "+to);
        if(!name || !to) throw new Error('Name or to required.');

        sendData({
            task: 'CHAT',
            payload: { name, to },
        });
    };

    const sendMessage = ({name, to, body}) => {
        //console.log("name: "+name+" to:"+to+" body: "+body);
        if(!name || !to || !body) throw new Error("Name or to or body required.");

        sendData({
            task: 'MESSAGE',
            payload: { name, to, body },
        });
    }
    
    const sendData = async (data) => {
        await client.send(JSON.stringify(data));
    };

    const closeWS = () => {
        sendData({
            task: 'CLOSE',
            payload: {},
        });
    }

    const clearMessages = () => {
        sendData(["clear"]);
    };

    const displayStatus = (s) => {
        if(s.msg){
            const {type, msg} = s;
            const content = {
                content: msg,
                duration: 1.0,
            };
            switch(type){
                case 'success':
                    message.success(content);
                    break;
                case 'error':
                default:
                    message.error(content);
                    break;
            }
        }
    }

    return (
        <ChatContext.Provider
            value={{
                status, me, signedIn, messages, hasInit, chatBoxes,
                setChatBoxes, setHasInit, setMe, setSignedIn, startChat, sendMessage, clearMessages, displayStatus,
                closeWS,
            }}
            {...props}
        />
    );
};


const useChat = () => useContext(ChatContext);

export { ChatProvider, useChat };