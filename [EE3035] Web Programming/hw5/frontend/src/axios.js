import axios from "axios";

const instance = axios.create({baseURL: 'http://localhost:4000/api/guess'});

const startGame = async () => {
    console.log("Begin to Start");
    const { data:{msg} } = await instance.post('/start');
    console.log(msg);
    return msg;
}

const guess = async (number) => {
    try{
        const { data:{msg} } = await instance.get('/guess', { params:{number}})
        return msg;
    }
    catch(error){
        return error.response.data.msg;
    }
}

const restart = async () => {
    console.log("Begin to Restart");
    const { data:{msg} } = await instance.post('/restart');
    console.log(msg);
    return msg;
}

export {startGame, guess, restart};