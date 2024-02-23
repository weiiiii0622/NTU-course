import { Router } from "express";
import ScoreCard from "../models/ScoreCard";

const router = Router();

// Utility Functions
const deleteDB = async () => {
    try {
        await ScoreCard.deleteMany({});
        console.log("Database deleted");
    } catch (e) { throw new Error("Database deletion failed"); }
};

const saveScoreCard = async (name, subject, score) => {
    console.log("add....");
    console.log("name: "+name+" sub: "+subject+" score: "+score);
    const existing = await ScoreCard.findOne({ name, subject });

    if(existing){
        await ScoreCard.updateOne({"name":name, "subject":subject}, { $set: { "score": score }});
        const curUser = await ScoreCard.find({name});
        let msg = `Updating (${name}, ${subject}, ${score})`;
        var rec = [];
        curUser.forEach((obj) => {
            rec.push(obj);
        })
        return {message:msg, card:false, record:rec};
    }
    else{
        try {
            const newScoreCard = new ScoreCard({ name, subject, score });
            await newScoreCard.save();
            const curUser = await ScoreCard.find({name});
            let msg = `Adding (${name}, ${subject}, ${score})`;
            var rec = [];
            curUser.forEach((obj) => {
                rec.push(obj);
            })
            return {message:msg, card:true, record:rec};
        } catch (e) { throw new Error("ScoreCard creation error: " + e); }
    }
};

const queryScoreCard = async (type, queryString) => {
    console.log("query....");
    console.log(queryString);
    if(type==="name") 
        var existing = await ScoreCard.find({ name : queryString });
    else 
        var existing = await ScoreCard.find({ subject : queryString });

    console.log(existing);
    if(existing.length){
        let msg = [];
        existing.forEach((obj) => {
            msg.push(obj);
        })
        return {messages:msg};
    }
    else{
        let msg = `${type} (${queryString}) not found!`;
        return {message:msg};
    }
};


// Clear
router.delete("/cards", async (req, res) => {
    await deleteDB();
    res.json({ message: 'Database cleared'});
});

// Add
router.post("/card", async (req, res) => {
    console.log("add req....");
    console.log(req);
    let msg = await saveScoreCard(req.body.name, req.body.subject, +req.body.score);
    res.json(msg);
});

// Query
router.post("/cards", async (req, res) => {
    console.log("query req....");
    console.log(req);
    let msg = await queryScoreCard(req.body.type, req.body.queryString);
    res.json(msg);
});

export default router;