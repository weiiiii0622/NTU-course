import express, { query } from "express";
import { genNumber, getNumber } from "../core/getNumber";

const isNumeric = (value) => {
    return /^-?\d+$/.test(value);
}

const router = express.Router();

router.post('/start', (_, res) => {
    genNumber();
    const ans = getNumber();
    res.json({ msg: `The game has started. Answer is: ${ans}`})
});

router.get('/guess', (req, res) => {
    const ans = getNumber();
    const n = req.query.number;
    if(isNumeric(n)){
        const num = parseInt(n);
        if(num>=1 && num<=100){
            if(num === ans){
                res.json({ msg: 'Equal'});
            }
            else if(num > ans){
                res.json({ msg: 'Smaller'});
            }
            else{
                res.json({ msg: 'Bigger'});
            }
        }
        else{
            res.status(406).send({ msg: `Error: "${n}" is not a valid number (1 - 100)` });
        }
    }
    else{
        res.status(406).send({ msg: `Error: "${n}" is not a valid number (1 - 100)` });
    }
});

router.post('/restart', (_, res) => {
    genNumber();
    const ans = getNumber();
    res.json({ msg: `The game has restarted. Answer is: ${ans}`})
});

export default router;