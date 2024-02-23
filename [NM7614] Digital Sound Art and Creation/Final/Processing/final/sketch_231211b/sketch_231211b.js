const { Engine, World, Bodies, Mouse, MouseConstraint, Constraint, Collision, Detector } = Matter;

// ********************************** PARAMETER **********************************
const DEBUG        = 1;
const FONT_SIZE    = 40;
const GAME_TIME    = 60; // seconds
const TOTAL_STAGE  = 2;

const BOX_SIZEX    = [85, 20, 203,  18, 80, 38, 70];
const BOX_SIZEY    = [20, 85,  18, 203, 38, 80, 70];

const SLING_XPOS   = 0.15;
const SLING_YPOS   = 0.65;

const SPACE_WIDTH  = 20;
const SPACE_HEIGHT = 8;

//const IP = "192.168.68.50";
const IP = "127.0.0.1";
const PORT = "8081";

// ********************************** Variables **********************************

// Game related
let curScene = 0; // 0: startmenu, 1: game, 2: gameover, 3:gamewin
let curStage = 1; // Default stage 1

// Menu
let gameOverBG;

// Object
let ground, slingshot, slingshot_copy, world, engine, mConstraint;
let collideDetector;

let bird;
let bird_left;

let pig_left;
let pigs = [];
let PIG_NUM;
let PIG_XPOS;
let PIG_YPOS;
let PIG_TYPE;

let boxes = [];
let BOX_NUM;
let BOX_XPOS;
let BOX_YPOS;
let BOX_TYPE;

let worldObj = [];
let worldObjPairs = [];

// Object Images
let RedBirdImg;
let pigImg = [];
let boxImg = [];
let bkgImg, bkgImg1, bkgImg2;

// Mouse
let isMouseOnBird = false;
let isBirdFlying = false;

// Font
let font;

// Timer
let isGameOverPending;
let isReset = 0;
let gameTimer;
let remainingTime;
let birdFlyCounter = 0;
let boxSoundCounter = 0;

// WebSocket

var oscPort;

// ********************************** Core Functions **********************************

function preload() {
  RedBirdImg = loadImage('./images/RedBird.png');
  pigImg[0] = loadImage('./images/pig-1.png');
  pigImg[1] = loadImage('./images/pig-2.png');
  pigImg[2] = loadImage('./images/pig-3.png');
  boxImg[0] = loadImage('./images/wood-1.png');    // short left-right
  boxImg[1] = loadImage('./images/wood-1-2.png');  // short top-down
  boxImg[2] = loadImage('./images/wood-2.png');    // long  lr
  boxImg[3] = loadImage('./images/wood-2-2.png');  // long  td
  boxImg[4] = loadImage('./images/wood-3.png');    // fat   lr
  boxImg[5] = loadImage('./images/wood-3-2.png');  // fat   td
  boxImg[6] = loadImage('./images/wood-4.png');  // fat   td
  startMenuImg = loadImage('./images/startmenu.jpg');
  winImg = loadImage('./images/win.jpg');
  bkgImg = loadImage('./images/background-2.png');
  bkgImg1 = loadImage('./images/background-21.png');
  bkgImg2 = loadImage('./images/background-22.png');
  groundImg = loadImage('./images/ground.jpg');
  
  font = loadFont('./assets/angrybirds-regular.ttf');
}

function setStage(stage){
  if(stage == 1){
    bird_left    = 3;
    pig_left     = 1;
    
    PIG_NUM      = 1;
    PIG_XPOS     = [185];
    PIG_YPOS     = [170];
    PIG_TYPE     = [0];
    
    BOX_NUM      = 5;
    BOX_XPOS     = [260, 240, 130, 110, 185];
    BOX_YPOS     = [0, 0, 0, 0, 85];
    BOX_TYPE     = [1, 1, 1, 1,  2];
  }
  else if(stage == 2){
    bird_left    = 3;
    pig_left     = 3;
    
    PIG_NUM      = 3;
    PIG_XPOS     = [185, 185, 475];
    PIG_YPOS     = [ 85, 180, 100];
    PIG_TYPE     = [  0,   1,   2];
    
    BOX_NUM      = 16;
    BOX_XPOS     = [260, 240, 130, 110, 185, 260, 240, 130, 110, 185, 400, 550, 400, 550, 475, 475];
    BOX_YPOS     = [  0,   0,   0,   0,  85, 105, 105, 105, 105, 170,   0,   0, 100, 100,  80, 160];
    BOX_TYPE     = [  1,   1,   1,   1,   2,   1,   1,   1,   1,   2,   5,   5,   6,   6,   2,   2];    
  }
  else if(stage == 3){
    
  }
  else{
    setStage(1);
  }
}

function setup() {
  const canvas = createCanvas(windowWidth, windowHeight);
  
  // Text
  textFont(font);
  textSize(FONT_SIZE);
  textAlign(CENTER, CENTER);
  
  // Menu
  gameOverBG = document.getElementById('gameover_bg');
  
  // Timer
  gameTimer = new Timer((GAME_TIME+1) * 1000, false);
  
  // Game Engine
  engine = Engine.create();
  world = engine.world;
  const mouse = Mouse.create(canvas.elt);
  const options = {
    mouse: mouse
  };
  mouse.pixelRatio = pixelDensity();
  mConstraint = MouseConstraint.create(engine, options);
  World.add(world, mConstraint);
 
  // Stage
  setStage(1);
 
  // Ground
  ground = new Ground(width / 2, height - 10, width*2, 20, groundImg);
  
  // Bird
  bird = new RedBird(width * SLING_XPOS, height * SLING_YPOS, 30, RedBirdImg);
  worldObj.push(bird);
  
  // Pig
  pigCount = PIG_NUM;
  pigs = [];
  for(let i = 0; i < pigCount; i++){
    pigs[i] = new Pig(width - PIG_XPOS[i], height-20 - PIG_YPOS[i], 30, pigImg[PIG_TYPE[i]]);
    worldObj.push(pigs[i]);
  }
  
  // Box
  boxes = [];
  for(let i = 0; i < BOX_NUM; i++){
    boxes[i] = new Box(width - BOX_XPOS[i], height-20 - BOX_YPOS[i], BOX_SIZEX[BOX_TYPE[i]], BOX_SIZEY[BOX_TYPE[i]], boxImg[BOX_TYPE[i]]);
    worldObj.push(boxes[i]);
  }
  
  // Slingshot
  slingshot = new SlingShot(width * (SLING_XPOS - 0.02), height * (SLING_YPOS + 0.02), bird.body);
  slingshot_copy = new SlingShot(width * (SLING_XPOS + 0.01), height * (SLING_YPOS + 0.02), bird.body);

  // Collision
  worldObj.push(ground);
  worldObjPairs = generatePairs(worldObj);

  worldObj.forEach(Obj => {
    console.log(Obj);
  });
  
  //gameTimer.reset();
  //gameTimer.start();
  
  // Socket
  connectSocket();
}

function reset(){
  console.log("Reset!");
  isReset = 1;
  isGameOverPending = false;
  isMouseOnBird = false;
  isBirdFlying = false;
  
  // Socket
  initSocket();
  
  // Menu
  gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 0)';
  
  // Reset
  World.remove(world, bird.body);
  for(let i = 0; i < PIG_NUM; i++){
    World.remove(world, pigs[i].body);
  }
  for(let i = 0; i < BOX_NUM; i++){
    World.remove(world, boxes[i].body);
  }
  
  // Update stage
  setStage(curStage);
  
  worldObj = [];
  // Add Bird
  bird = new RedBird(220, 490, 30, RedBirdImg);
  worldObj[0] = bird;
  slingshot.attach(bird.body);
  slingshot_copy.attach(bird.body);
  
  // Add Pig
  pigs = [];
  for(let i = 0; i < PIG_NUM; i++){
    pigs[i] = new Pig(width - PIG_XPOS[i], height-20 - PIG_YPOS[i], 30, pigImg[PIG_TYPE[i]]);
    worldObj.push(pigs[i]);
  }
  
  // Add Box
  boxes = [];
  for(let i = 0; i < BOX_NUM; i++){
    boxes[i] = new Box(width - BOX_XPOS[i], height-20 - BOX_YPOS[i], BOX_SIZEX[BOX_TYPE[i]], BOX_SIZEY[BOX_TYPE[i]], boxImg[BOX_TYPE[i]]);
    worldObj.push(boxes[i]);
  }
  worldObj.push(ground);
  console.log(worldObj);
  worldObjPairs = generatePairs(worldObj);
  
  setTimeout(() => {
    gameOverBG.style.height = '0vh';
  }, 1000);
  
  // Timer
  gameTimer.reset();
  gameTimer.start();
  
  isReset = 0;
}


function keyPressed() {
  
  if(curScene == 0){
    // Start Menu
    if (keyCode == ENTER){
      console.log("Pressed ENTER");
      curStage = 1;
      curScene = 1;
      reset();
    }
  }
  else if(curScene == 1){
    // Game
    if (key == ' ') {
      if(!slingshot.isAttach() && bird_left > 0){
        // slingshot is now empty
        World.remove(world, bird.body);
        bird = new RedBird(220, 490, 30, RedBirdImg);
        // Regenerate pairs for collision detection since the bird is a new object
        worldObj[0] = bird;
        worldObjPairs = generatePairs(worldObj);
        slingshot.attach(bird.body);
        slingshot_copy.attach(bird.body);
       
      }
    }
    else if(key == 'r'){
      gameOverBG.style.height = '100vh';
      gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 1.0)';
      setTimeout(() => {
        curStage = 1;
        curScene = 1;
        reset();
      }, 1000);
    }
  }
  else if(curScene == 2){
    // Game Over
    initSocket();
    if (key == 'r'){
      gameOverBG.style.height = '100vh';
      gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 1.0)';
      setTimeout(() => {
        curStage = 1;
        curScene = 1;
        reset();
      }, 1500);
    }
  }
  else if(curScene == 3){
    // Game Win
    initSocket();
    if (key == 'r'){
      //gameOverBG.style.height = '100vh';
      console.log("Restart Game.");
      gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 1.0)';
      setTimeout(() => {
        curStage = 1;
        curScene = 1;
        reset();
      }, 1500);
    }
  }
}

function mousePressed() {
  //console.log(mouseX, mouseY);
  //console.log("Bird", bird.body.position);
  // Check if the mouse is inside the div boundaries
  if (
    mouseX >= bird.body.position.x - bird.r &&
    mouseX <= bird.body.position.x + bird.r &&
    mouseY >= bird.body.position.y - bird.r &&
    mouseY <= bird.body.position.y + bird.r
  ) {
    if(slingshot.isAttach() && isMouseOnBird == false){  
      // Send OSC slingshot
      let pos = {"x": width * SLING_XPOS, "y":height * SLING_YPOS};
      let newPos = mapPosition(pos);
      oscPort.send(createPacket("/slingshot", 0, newPos.x, newPos.y, 0, 0));
      isMouseOnBird = true;
    }
    
    //console.log("Mouse is pressed on the bird!");
  }
}

function mouseReleased() {
  if(slingshot.isAttach() && isMouseOnBird == true){  
    setTimeout(() => {
      let pos = {"x": width * SLING_XPOS, "y":height * SLING_YPOS};
      let newPos = mapPosition(pos);
      bird_left -= 1;
      slingshot.fly();
      slingshot_copy.fly();
      oscPort.send(createPacket("/slingshot", 1, newPos.x, newPos.y, 0, 0));
      isMouseOnBird = false;
      isBirdFlying = true;
    }, 50);
  }
}

function draw() {
  //console.log("Current Scene: ", curScene);
  //console.log("Current Stage: ", curStage);
  
  if(curScene == 0){
    background(startMenuImg);
    str = 'Press Enter to Start!';
    fill(255, 255, 255);
    text(str, width/2, height*3/4);
  }
  else if(curScene == 1){
    // Game
    background(bkgImg);
    stroke(0);

    // Object Update
    Engine.update(engine);
    ground.show();
    for (let box of boxes) {
      box.show();
    }
    for (let pig of pigs) {
      if(!pig.isDead){
        pig.show();
        // Pig Out of Bound ->
        newPos = mapPosition(pig.body.position)
        if(newPos.x < (-SPACE_WIDTH)/2 || newPos.x >(SPACE_WIDTH)/2 || newPos.y < 0 || newPos.y > SPACE_HEIGHT){
          pig.isDead = true;
          pig_left -= 1;
          worldObj = worldObj.filter(obj => obj != pig);
          worldObjPairs = generatePairs(worldObj);
          console.log(`${pig_left} pigs left.`);
          oscPort.send(createPacket("/pig", 0, newPos.x, newPos.y, 0, 0));
        }
      }
    }
    bird.show();
    slingshot.show();
    slingshot_copy.show();
    detectCollision();
    
    // Get Bird Location
    if(isBirdFlying == true){
      birdFlyCounter -= 1;
      
      if(birdFlyCounter <= 0){
        birdFlyCounter = 5;
        console.log("Bird Flying!");
        newPos = mapPosition(bird.body.position)
        // Bird x-speed: 4 * bird.body.velocity.x * bird.body.deltaTime)
        let birdXSpeed_px = 4 * bird.body.velocity.x * bird.body.deltaTime;
        let birdYSpeed_px = 4 * bird.body.velocity.y * bird.body.deltaTime;
        oscPort.send(createPacket("/bird", 0, newPos.x, newPos.y, SPACE_WIDTH * birdXSpeed_px/width, SPACE_HEIGHT * birdYSpeed_px/width));
      }
      
      // Out Of Bound
      if(newPos.x < (-SPACE_WIDTH)/2 || newPos.x >(SPACE_WIDTH)/2 || newPos.y < 0){
        isBirdFlying = false;
        oscPort.send(createPacket("/bird", 1, newPos.x, newPos.y, 0, 0));
      }
    }
    
    
    // Game Win
    if(pig_left <= 0){
      if(curStage == TOTAL_STAGE){
        // Finish!
        if(!isGameOverPending){
          gameTimer.pause();
          isBirdFlying = false;
          isGameOverPending = true;
          console.log("Game Win.");
          gameOverBG.style.height = '100vh';
          gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 1.0)';
          setTimeout(() => {
            curScene = 3;
            gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 0)';
          }, 1500);
        }
      }
      else{
        // Next Stage
        if(!isGameOverPending){
          gameTimer.pause();
          isBirdFlying = false;
          isGameOverPending = true;
          curStage += 1;
          console.log("Going to stage", curStage);
          gameOverBG.style.height = '100vh';
          gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 1.0)';
          setTimeout(() => {
            reset();
          }, 1500);
        }
      }
    }
    
    // Timer
    fill(0);
    remainingTime = int(gameTimer.getRemainingTime() / 1000);
    
    if((remainingTime <= 0 || bird_left <= 0) && pig_left > 0 && !isGameOverPending){
      // Game Over
      isBirdFlying = false;
      isGameOverPending = true;
      setTimeout(() => {
        console.log("Game Over.");
        gameOverBG.style.height = '100vh';
        gameOverBG.style.backgroundColor = 'rgba(0, 0, 0, 0.35)';
      }, 5000);
      setTimeout(() => {
        curScene = 2;
      }, 5300);
    }
    str = 'R e m a i n i n g : ' + remainingTime;
    text(str, 180, 50);
    str = 'B i r d : ' + bird_left;
    text(str, width - 180, 50);
    
  }
  else if(curScene == 2){
    // Game Over
    str = 'Press R to retry ...';
    fill(255, 0, 0);
    text(str, width/2, height/2);
  }
  else if(curScene == 3){
    // Game Win
    background(winImg);
    
    str = 'You Win ! Press R to Restart!';
    fill(255, 255, 255);
    text(str, width/2, height*3/4);
  }
}


// ********************************** Socket Related **********************************
function mapPosition(pos){
   // map position to x:[-10, 10], y: [0, 8], from bottom-left to top-right
   return {"x": ((-SPACE_WIDTH)/2 + SPACE_WIDTH * pos.x/width), "y": SPACE_HEIGHT - (0 + SPACE_HEIGHT * pos.y/height)};
}

function createPacket(address, isActive, x_coord, y_coord, x_speed, y_speed) {
  return {
        address: address,
        args: [
                {
                    type: "f",
                    value: isActive
                },
                {
                    type: "f",
                    value: x_coord
                },
                {
                    type: "f",
                    value: y_coord
                },
                {
                    type: "f",
                    value: x_speed
                },
                {
                    type: "f",
                    value: y_speed
                },
        ]
    };
}

function initSocket(){
 // oscPort.send(createPacket("/wood", 1, 0, 0, 0, 0));
  oscPort.send(createPacket("/bird", 1, 0, 0, 0, 0));
 // oscPort.send(createPacket("/pig",  1, 0, 0, 0, 0));
  oscPort.send(createPacket("/slingshot",  1, 0, 0, 0, 0));
}

function connectSocket(){
    oscPort = new osc.WebSocketPort({
      url: "ws://"+IP+":"+PORT // URL to your Web Socket server.
    });
    
    
    console.log(oscPort)
    oscPort.open();
    oscPort.on("ready", function () {
      console.log("connected");
      initSocket();
    });
    oscPort.on("error", function (e) {
      console.log("error:")
      console.log(e.message)
    });
}

// ********************************** Game Related **********************************
function createMenuDiv(id){
  myDiv = createDiv('');
  myDiv.id('myDiv');
  myDiv.position(0, 0);
  myDiv.style('width', '100vw');
  myDiv.style('height', '100vh');
  myDiv.style('background-color', 'rgba(0, 0, 0, 0)');
  return myDiv;
}

function generatePairs(list) {
  let pairs = [];
  for (let i = 0; i < list.length - 1; i++) {
    for (let j = i + 1; j < list.length; j++) {
      if((list[i].type=="pig" && list[j].type!="ground") || (list[j].type=="pig" && list[i].type!="ground") || (list[i].type=="box" && list[j].type=="box")){
        continue;
      }
      pairs.push([list[i], list[j]]);
    }
  }
  //console.log(pairs);
  return pairs;
}

function checkValidCollision(obj1, obj2){
  // Collided
  const speed_thres = 0.7;
  if(Collision.collides(obj1.body, obj2.body) != null){
    // Some condtion that they are newly collided
    if(obj1.type == "pig" && obj2.type == "ground" || obj1.type == "ground" && obj2.type == "pig"){
      return true;
    }
    if((obj1.type == "ground" || obj1.body.speed > speed_thres) && (obj2.type == "ground" || obj2.body.speed > speed_thres)){
      return true;
    }
  }
  return false;
}

function handleCollision(obj1, obj2){
   //if(DEBUG) console.log(obj1.type , obj2.type, "collided!", obj1.body.speed, obj2.body.speed);
   
   if(obj1.type == "box" || obj2.type == "box"){    
     // box collide with anything -> box sound
     if(obj1.type == "box"){newPos = mapPosition(obj1.body.position);}
     if(obj2.type == "box"){newPos = mapPosition(obj2.body.position);}
     
     // Send OSC packet
     if(boxSoundCounter < 5){
       //console.log("Box Sound!");
       boxSoundCounter += 1;
       oscPort.send(createPacket("/wood", 0, newPos.x, newPos.y, 0, 0));
       setTimeout(() => {
        boxSoundCounter -= 1;
       }, 1000);
     }
   }
   if(obj1.type == "bird" || obj2.type == "bird"){ 
     // bird collide -> bird sound stop
     console.log("Bird crashed ...");
     isBirdFlying = false;
    
     // Send OSC packet
     if(obj1.type == "bird"){newPos = mapPosition(obj1.body.position);}
     if(obj2.type == "bird"){newPos = mapPosition(obj2.body.position);}
     
     oscPort.send(createPacket("/bird", 1, newPos.x, newPos.y, 0, 0));
   }
   if(obj1.type == "pig" && obj2.type == "ground" || obj1.type == "ground" && obj2.type == "pig"){
     // pig collide with ground -> pig die
     if(obj1.type=="pig"){
       obj1.isDead = true;
       pig_left -= 1;
       worldObj = worldObj.filter(obj => obj != obj1);
       worldObjPairs = generatePairs(worldObj);
       console.log(`${pig_left} pigs left.`);
       
       newPos = mapPosition(obj1.body.position);
       oscPort.send(createPacket("/pig", 0, newPos.x, newPos.y, 0, 0));
     }
   }
}

function detectCollision() {
  worldObjPairs.forEach(pairs => {
    if(checkValidCollision(pairs[0], pairs[1])){
      handleCollision(pairs[0], pairs[1]);
    }
  });
}
