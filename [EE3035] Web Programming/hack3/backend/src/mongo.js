import mongoose from "mongoose";
import { dataInit } from "./upload.js";

import "dotenv-defaults/config.js";
import { config } from "dotenv-defaults";

config();
//console.log(process.env.PORT);
mongoose.set("strictQuery", true);

async function connect() {
  // TODO 1 Connect to your MongoDB and call dataInit()
  mongoose.connect(
    // TODO Part I-3: connect the backend to mongoDB
    process.env.MONGO_URL, {
        useNewUrlParser: true,
        useUnifiedTopology: true,
    }
  ).then(async res => {
      if (process.env.MODE === 'Reset') {
          console.log('Reset Mode: reset the data')
          dataInit()
      }
      console.log("mongo db connection created")
  })
  // TODO 1 End
}

export default { connect };
