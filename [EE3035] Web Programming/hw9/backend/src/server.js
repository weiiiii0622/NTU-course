import express from 'express';
import bodyParser from 'body-parser';
import mongoose from 'mongoose';
import dotenv from "dotenv-defaults";
import cors from 'cors';

import db from './db'
import ScoreCard from './models/ScoreCard'
import routes from './routes';
import path from 'path';


const app = express();

if (process.env.NODE_ENV === "production") {
    const __dirname = path.resolve();
    app.use(express.static(path.join(__dirname, "../frontend", "build")));
    app.get("/*", function (req, res) {
      res.sendFile(path.join(__dirname, "../frontend", "build", "index.html"));
    });
}
if (process.env.NODE_ENV === "development") {
	app.use(cors());
}
app.use(express.json());
app.use('/', routes);
app.use(bodyParser.json());
const port = process.env.PORT || 4000;

db.connect();

// const saveUser = async (id, name) => {
//     const existing = await User.findOne({ name });

//     if (existing) throw new Error(`data ${name} exists!!`);
//     try {
//         const newUser = new User({ id, name });
//         console.log("Created user", newUser);
//         return newUser.save();
//     } catch (e) { throw new Error("User creation error: " + e); }
// };
// const deleteDB = async () => {
//     try {
//         await User.deleteMany({});
//         console.log("Database deleted");
//     } catch (e) { throw new Error("Database deletion failed"); }
// };

// const db = mongoose.connection;
// db.on("error", (err) => console.log(err));
// db.once("open", async () => {
//     await deleteDB();
// });


// app.get('/', (req, res) => {
//  res.send('Hello, World!');
// });

// app.get('/', (req, res) => {
//     res.send('Received a GET HTTP method');
// });
// app.post('/', (req, res) => {
//     console.log(req);
//     res.send('Received a POST HTTP method');
// });
// app.put('/', (req, res) => {
//     res.send('Received a PUT HTTP method');
// });
// app.delete('/', (req, res) => {
//     res.send('Received a DELETE HTTP method');
// });
// app.post('/users', (req, res) => {
//     res.send('POST HTTP method on users resource');
// });
   
// app.put('/users/:userId', (req, res) => {
//     res.send(
//     `PUT HTTP method on users/${req.params.userId} resource`,
//     );
// });

app.listen(port, () =>
    console.log(`Example app listening on port ${port}!`),
);