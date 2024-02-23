var num;

const genNumber = () => {
    num = Math.floor(Math.random()*100)+1;
}

const getNumber = () => {
    return num;
}

export {genNumber, getNumber};