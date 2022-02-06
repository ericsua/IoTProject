const express = require('express')
const bodyParser = require('body-parser');
const cors = require('cors');

const TelegramBot = require('node-telegram-bot-api');

// replace the value below with the Telegram token you receive from @BotFather
//we will use obj.token and obj.myChatID (token of telegram bot and chatID of the person to send the messages)
var obj = require("./secrets.js")

const app = express();
const port = 3000;



let tmstmp = Date.now();


let datas = "60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60";

app.use(cors());


app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

app.post('/api', (req, res) => {
    const body = req.body;


    console.log(body.data);
    tmp = ""+body.data;
    let tmpArr = tmp.split(',');
    tmpArr.pop();
    
    //books.push(book);

    //console.log("seconds: " + secondsAgo(tmstmp));

    //If 10 seconds are passed since last message has been sent, check the noise
    if (secondsAgo(tmstmp) > 10) {
        //console.log("10 sec passed");
        let maxNum = 0;
        tmpArr.forEach(x => {
            if(parseInt(x) > 90) {
                maxNum+=1;
            }
        });

        //console.log("MaxNum: " + maxNum);
        if (maxNum > 10) {
            bot.sendMessage(obj.myChatID, "⚠️🔔🔊 ATTENTION! Loud noises were detected in your room!");
            tmstmp = Date.now()
        }
    }

    //stringify the array with the data to be sent
    datas = tmpArr.join();

    res.send('Data is added to the database');
});
let i = 0;
app.get('/api', (req, res) => {
    const header = req.query;
    const params = req.params;


    console.log(req.body);
    //books.push(book);

    if (datas == "")
        res.send('Hi!');
    else{
        //send the response as a json object
        res.json({"id": i,"timestamp": Date.now(), "data": datas})
        i+=1
        
    }
        
});

server = app.listen(port, () => console.log(`Hello world app listening on port ${port}!`));

server.keepAliveTimeout = 300000000;

function secondsAgo(unixTime) {
    //return Math.round((new Date().getTime() / 1000)) - unixTime;
    return Math.round((Date.now() - unixTime) / 1000)
}


//------------ BOT TELEGRAM ---------------------------



// Create a bot that uses 'polling' to fetch new updates
const bot = new TelegramBot(obj.token, {polling: true});

// Send response to start command
bot.onText(/\/start/, (msg) => {
  // 'msg' is the received Message from Telegram
  // 'match' is the result of executing the regexp above on the text content
  // of the message

  const chatId = msg.chat.id;
  const resp = "Hi!\n\nThis is a bot made for the project of Embedded Systems @ UniTN by Eric.\n\nIt notifies you with a message when the MSP432 detects too much noise in your room."
  console.log(chatId)

  // send back the matched "whatever" to the chat
  bot.sendMessage(chatId, resp);
});


// Listen for any kind of message. There are different kinds of
// messages.
bot.on('message', (msg) => {
  const chatId = msg.chat.id;
  if (msg.text != "/start")
        // send a message to the chat acknowledging receipt of their message
        bot.sendMessage(chatId, 'Sorry, no commands are allowed.');
});