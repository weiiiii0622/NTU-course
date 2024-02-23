import "./App.css";
import styled from 'styled-components';
import { useState, useEffect, useRef } from "react";
import { useChat } from "./hooks/useChat";

import SignIn from "./containers/SignIn";
import ChatRoom from "./containers/ChatRoom";

const Wrapper = styled.div`
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    height: 100vh;
    width: 500px;
    margin: auto;
`;

const App = () => {
    const { status, me, signedIn, displayStatus } = useChat();
    
    useEffect(() => {
        displayStatus(status);
    }, [status]);

    return (
        <Wrapper> {signedIn? <ChatRoom />: <SignIn  me={me}/>} </Wrapper>
        //<Wrapper> {signedIn? <SignIn />: <SignIn />} </Wrapper>
    )
}

export default App;
