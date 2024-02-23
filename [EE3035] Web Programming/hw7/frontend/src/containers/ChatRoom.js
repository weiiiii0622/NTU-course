import "../App.css"
import styled from 'styled-components';
import { useState, useEffect, useRef } from "react";
import { Button, Input, Tabs, Tag} from "antd";

import { useChat } from '../hooks/useChat';
import Title from '../components/Title';
import Message from '../components/Message';
import ChatModal from '../components/ChatModal';
// import { start } from "repl";

const FootRef = styled.div`
    height: 20px;
`;

const ChatBoxesWrapper = styled(Tabs)`
    width: 100%;
    height: 300px;
    background: #eeeeee52;
    border-radius: 10px;
    margin: 20px;
    padding: 20px;
    overflow: scroll;
`;

const ChatRoom = () => {
    const {me, chatBoxes, hasInit, messages, displayStatus, setHasInit, setChatBoxes, sendMessage, startChat, closeWS } = useChat();
    
    const [activeKey, setActiveKey] = useState('');
    const [msg, setMsg] = useState(''); // text input body
    const [msgSent, setMsgSent] = useState(false);
    const [modalOpen, setModalOpen] = useState(false);
    const [friend, setFriend] = useState('');
    // const [username, setUsername] = useState('');
    // const [body, setBody] = useState('');

    const bodyRef = useRef(null);
    const msgFooter = useRef(null);

    

    const displayChat = (chat) => (
        chat.length === 0 ? (
            <p style={{ color: "#ccc" }}>No messages...</p>
        ) : (
            <>
                {chat.map(({name, body}, i) => (<Message name={name} isMe={name===me} message={body} key={i}/>))}
                <FootRef ref={msgFooter} />
            </>
        )
    )
    const extractChat = () => {
        return displayChat(
            messages
        );
    };

    const createChatBox = (friend) => {
        if(chatBoxes.some(({key}) => key === friend)){
            throw new Error(friend+"'s chat box has already opened.");
        }
        const chat = extractChat();
        //console.log(chat);
        //setChatBoxes([...chatBoxes, {label: friend, children: chat, key: friend}]);
        //setMsgSent(true);
        return friend;
    };

    const removeChatBox = (targetKey, activeKey) => {
        const index = chatBoxes.findIndex(({key}) => key === activeKey);
        const newChatBoxes = chatBoxes.filter(({key}) => key !== targetKey);
        setChatBoxes(newChatBoxes);

        return activeKey
            ? activeKey === targetKey
                ? index === 0
                    ? ''
                    : chatBoxes[index-1].key
                : activeKey
            : '';
    };

    const scrollToBottom = () => {
        msgFooter.current?.scrollIntoView
        ({ behavior: 'smooth', block: "start" });
    };

    useEffect(() => {
        scrollToBottom();
        setMsgSent(false);
    }, [msgSent]);

    useEffect(() => {
        console.log("Rerender messages.");
        console.log(messages);
        console.log("Init "+friend);
        
        if(!hasInit) {
            setHasInit(true);
        }
        else{
            var idx = chatBoxes.findIndex(el => el.key===friend);
            if(chatBoxes[idx]){
                let newBoxes = [...chatBoxes];
                newBoxes[idx] = {label: friend, children: extractChat(messages), key: friend};
                setChatBoxes([...newBoxes]);
                //setFriend("null");
                setMsgSent(true);
            }
            else{
                setChatBoxes([...chatBoxes, {label: friend, children: extractChat(messages), key: friend}]);
                setMsgSent(true);
            }
        }
    }, [messages]);

    return (
        <>
            <Title name={me}/>
            <>
                <ChatBoxesWrapper
                    tabBarStyle={{ height: '36px' }}
                    type="editable-card"
                    activeKey={activeKey}
                    onChange={async (key)=>{
                        setFriend(key);
                        await startChat({name:me, to:key});

                        setActiveKey(key);

                    }}
                    onEdit={(targetKey, action) => {
                        if(action === 'add') setModalOpen(true);
                        else if(action === 'remove'){
                            closeWS();
                            setActiveKey(removeChatBox(targetKey, activeKey));
                        }
                    }}
                    items = {chatBoxes}
                />
                
                <ChatModal
                    open={modalOpen}
                    onCreate={async ({ name }) => {
                        setFriend(name);
                        await startChat({name:me, to:name});
                        console.log("init message:");
                        console.log(messages);
                        setModalOpen(false);
                        setActiveKey(createChatBox(name));
                    }}
                    onCancel={()=>{
                        setModalOpen(false);
                    }}   
                />
                
            </>
            
            <Input.Search
                ref={bodyRef}
                value={msg}
                onChange={(e)=>{setMsg(e.target.value)}}
                onSearch={(msg) => {
                    if(!msg){
                        displayStatus({
                            type: 'error',
                            msg: 'Please enter a username and a message body.'
                        });
                        return;
                    }
                    console.log(msg);
                    sendMessage({name:me, to:activeKey, body:msg});
                    setMsgSent(true);
                    setMsg('');    
                }}
                enterButton="Send"
                placeholder="Type a message here..."
            ></Input.Search>
        
        </>
    );
}

export default ChatRoom;

// function App() {
//     const {status, messages, sendMessage, clearMessages} = useChat();
//     const [username, setUsername] = useState('');
//     const [body, setBody] = useState('');

//     const bodyRef = useRef(null);

//     const displayStatus = (s) => {
//         if(s.msg){
//             const {type, msg} = s;
//             const content = {
//                 content: msg,
//                 duration: 1.0,
//             };
//             switch(type){
//                 case 'success':
//                     message.success(content);
//                     break;
//                 case 'error':
//                 default:
//                     message.error(content);
//                     break;
//             }
//         }
//     }
//     useEffect(() => {
//         displayStatus(status);
//     }, [status]);

//     return (
//         <Wrapper>
//         {/* //<div className="App"> */}
//             <div className="App-title">
//                 <h1>Simple Chat</h1>
//                 <Button type="primary" danger onClick={clearMessages}>
//                     Clear
//                 </Button>
//             </div>

//             <div className="App-messages">
//                 {
//                     messages.length === 0 ?
//                         (<p style={{ color: "#ccc" }}>No messages...</p>)
//                     :
//                         (messages.map(({name, body}, i) => (
//                             <p className="App-message" key={i}>
//                                 <Tag color="blue"> {name} </Tag> {body}
//                             </p>
//                         )))
//                 }
//             </div>
//             <Input 
//                 placeholder="Username"
//                 value={username}
//                 onChange={(e)=>{setUsername(e.target.value)}}
//                 onKeyDown={(e)=>{
//                     if(e.key === 'Enter'){
//                         bodyRef.current.focus();
//                     }
//                 }}
//                 style={{ marginBottom: 10 }}
//             ></Input>
//             <Input.Search
//                 ref={bodyRef}
//                 value={body}
//                 onChange={(e)=>{setBody(e.target.value)}}
//                 onSearch={(msg) => {
//                     if(!msg || !username){
//                         displayStatus({
//                             type: 'error',
//                             msg: 'Please enter a username and a message body.'
//                         });
//                         return;
//                     }
//                     sendMessage({name:username, body:msg});
//                     setBody('');    
//                 }}
//                 enterButton="Send"
//                 placeholder="Type a message here..."
//             ></Input.Search>
//         </Wrapper>
//     );
// }