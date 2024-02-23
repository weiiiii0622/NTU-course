import styled from 'styled-components';
import { useState, useEffect, useRef } from "react";

import Title from "../components/Title"
import LogIn from '../components/LogIn';
import { useChat } from '../hooks/useChat';

const SignIn = ({me}) => {
    const { setMe, setSignedIn, displayStatus } = useChat();

    const handleLogin = (name) => {
        //console.log(name);
        if(!name){
            displayStatus({
                type: "error",
                msg: "Missing user name.",
            });
        }
        else{
            setSignedIn(true);
        }
    }

    return (
        <>
            <Title/>
            <LogIn me={me} setName={setMe} onLogin={handleLogin} />
        </>
    );
}

export default SignIn;