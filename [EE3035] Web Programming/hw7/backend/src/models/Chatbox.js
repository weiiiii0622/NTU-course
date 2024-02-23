import mongoose from 'mongoose';

const Schema = mongoose.Schema;

const ChatBoxSchema = new Schema({
    name: { type: String, required: [true, 'Name field is required.'] },
    users: [{ type: mongoose.Types.ObjectId, ref: 'User' }],
    messages: [{ type: mongoose.Types.ObjectId, ref: 'Message' }],
});
const ChatBoxModel = mongoose.model('ChatBox', ChatBoxSchema);

export default ChatBoxModel;
//export default ChatBoxModel;