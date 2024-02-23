import mongoose from 'mongoose';

const Schema = mongoose.Schema;

const UserSchema = new Schema({
    name: { type: String, required: [true, 'Name field is required.'] },
    chatBoxes: [{ type: mongoose.Types.ObjectId, ref: 'ChatBox' }],
});
const UserModel = mongoose.model('User', UserSchema);

export default UserModel;