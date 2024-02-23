import mongoose from 'mongoose'

const Schema = mongoose.Schema;
const ScoreCardSchema = new Schema({
    name: String, // Number is shorthand for {type: Number}
    subject: String,
    score: Number,
});
const ScoreCard = mongoose.model('ScoreCard', ScoreCardSchema);

export default ScoreCard;