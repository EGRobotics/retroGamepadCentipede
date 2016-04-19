#include "U8glib.h"
#define MAXLENGTH 10
#define MAXSHOTS 7
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // initialize u8g library for our display

// Variables for the refresh rate
  long startTime = 0; // millis() must be greater than this to run program (hard pause)
  long pauseAfter = 0; // program will pause for 1000ms when millis() reaches this number
  int temp = 0; 
  int gameover =0; 
  
// Mushroom Variables
  int mushroomx[MAXLENGTH]; 
  int mushroomy[MAXLENGTH]; 
  int mushroomcount = 9; 
  
// Shot Variables; 
int shotsFired  =0;
int shotsx[MAXSHOTS];   
int shotsy[MAXSHOTS]; 
  
// centipede variables
  int centix[MAXLENGTH]; 
  int centiy[MAXLENGTH];
  float dx = 1; // centipede horizontal direction
  int br = 2; // centipede body radius
  int centiLength = 9;
  float vprime = 2; // default velocity
  float v = vprime; // active velocity (initialized to default)
  //float dv = 0.1; // delta velocity (rate of ball accelleration after paddle hit)
  float mv = 10; // max velocity
// Ship Variables
  float p1s = 7; // ship size initialized to default
  float p1y = 60; // ships y coords
  float p1x = 64; // ships x coords
  long p1score = 0; // player 1's score
// Gamepad Controls  
  int DPAD_PUSH = 11, DPAD_DOWN = 10, DPAD_RIGHT = 9, DPAD_UP = 8, DPAD_LEFT = 7, A = 12, B = 13, POT_HIGH = 2, POT_LOW = 3, POT = 3, potRead = 0;
  bool pushIsPressed = false, downIsPressed = false, rightIsPressed = false, upIsPressed = false, leftIsPressed = false, aIsPressed = false, bIsPressed = false;

void setup() {
  pinMode(POT_HIGH, OUTPUT);
  pinMode(POT_LOW, OUTPUT);
  pinMode(DPAD_PUSH, INPUT_PULLUP);
  pinMode(DPAD_DOWN, INPUT_PULLUP);
  pinMode(DPAD_RIGHT, INPUT_PULLUP);
  pinMode(DPAD_UP, INPUT_PULLUP);
  pinMode(DPAD_LEFT, INPUT_PULLUP);
  pinMode(A, INPUT_PULLUP);
  pinMode(B, INPUT_PULLUP);
// randomSeed is neccessary to get varying random numbers each time program starts
  int rand = analogRead(0);
  randomSeed(rand);
  dx = 1; 
  centix[0] = centiLength*br; // head X coord (center default)
  centiy[0] = 2; // head Y coord (center default)

for (int i= 1; i < centiLength; i+=1) {
    centix[i] =centix[i-1]-(br); 
    centiy[i] =centiy[i-1];  
}

  for(int i = 0 ; i <=mushroomcount; i+=1){
    mushroomx[i]=random(10,118); 
    mushroomy[i]=random(10,54); 
  }
}

void hook(int buttonPin, bool& buttonState, String buttonName){
  if(digitalRead(buttonPin) == LOW && !buttonState){
    buttonState = true;
  }
  if(digitalRead(buttonPin) == HIGH && buttonState){
    buttonState = false;
  }
}

void buttonHooks(){
  hook(DPAD_PUSH, pushIsPressed, "DPAD");
  hook(DPAD_DOWN, downIsPressed, "DPAD DOWN");
  hook(DPAD_RIGHT, rightIsPressed, "DPAD RIGHT");
  hook(DPAD_UP, upIsPressed, "DPAD UP");
  hook(DPAD_LEFT, leftIsPressed, "DPAD LEFT");
  hook(A, aIsPressed, "A");
  hook(B, bIsPressed, "B");
}

void potHook(){
  int reading = analogRead(POT);
  if(reading - potRead > 5 || reading - potRead < -5){
    potRead = reading;
  }
  digitalWrite(POT_HIGH, HIGH);
  digitalWrite(POT_LOW, LOW);
}

void draw(void) {
  u8g.drawHLine(0, 0, 128);
  u8g.drawHLine(0, 63, 128);
  
  if (gameover == -1){
// move text input position
  u8g.setFont(u8g_font_osb18);
  temp = (128-u8g.getStrWidth("Try Again"))/2;  
  u8g.setPrintPos(temp, 50);
// print player time
  u8g.print("Try Again");
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(centix[0],centiy[0]); 
  u8g.print("Press B to Continue.."); 
  }else if (gameover == 1){
  // set score keeping font
  u8g.setFont(u8g_font_5x7);
  // move text input position
  u8g.setPrintPos(10, 9);
  // print player 1's score
  u8g.print(p1score);
  
  // move text input position
  u8g.setFont(u8g_font_osb18);
  temp = (128-u8g.getStrWidth("You Win"))/2;  
  u8g.setPrintPos(temp, 50);
  // print player time
  u8g.print("You Win!");
  u8g.setFont(u8g_font_5x7);
  u8g.setPrintPos(centix[0],centiy[0]); 
  u8g.print("Press B to Continue.."); 
  }else{
  
  // set score keeping font
  u8g.setFont(u8g_font_5x7);
  // move text input position
  u8g.setPrintPos(10, 9);
  // print player 1's score
  u8g.print(p1score);
  // draw ship
  u8g.drawBox(round(p1x), round(p1y), round(p1s),5);

  // draw centipede
for (int i = 0; i < centiLength; i+=1) {
    u8g.drawDisc(round(centix[i]), round(centiy[i]), br);
}

  // draw shots
for (int i = 0; i < shotsFired ;   i+=1) {
    u8g.drawDisc(shotsx[i], shotsy[i], 1);
}
// draw mushrooms
for (int i = 0; i < mushroomcount; i+=1){
  u8g.drawBox(mushroomx[i],mushroomy[i],3,3); 
}
  }
}
 
 
  
// Reset Centipede and Game Variables
  void reset(){ 
  centiLength = 9; 
  // reset centipede velocity
  if (gameover == -1){
    p1score = 0; 
    v = vprime; 
    } else{
    v+=1; }
  
  dx = 1;   
  centix[0] = centiLength*br; // head X coord (center default)
  centiy[0] = 2; // head Y coord (center default)

for (int i= 1; i < centiLength; i+=1) {
    centix[i] =centix[i-1]-br; 
    centiy[i] =centiy[i-1];  
}
  gameover = 0; 
  startTime = millis() + 500; // pause for 500ms (before ball reset is drawn on screen)
  pauseAfter = millis() + 520; // pause for 1000ms after 520ms (after ball reset is drawn on screen)


  for(int i = 0 ; i < MAXLENGTH; i+=1){
    mushroomx[i]=random(10,118); 
    mushroomy[i]=random(10,54); 
  }
  mushroomcount = 9; 

}

// returns the index of the mushroom collided with or -1 if none
int mushroomhit(int x, int y) {
 if (mushroomcount >=1){
  for(int i = 0; i <= mushroomcount; i+=1){
 
 if(abs(x-mushroomx[i]) < br && abs(y-mushroomy[i]) < br){
 return i; 
 }
}
}
return -1; 
}

// returns the 1 if the centipede is hit
int centihit(int x, int y) {
 for(int i = 0; i < centiLength; i+=1){
 
 if(abs(x-centix[i]) < br && abs(y-centiy[i]) < br){
 centiLength -=1; 
 return 1; 
 }
}
return 0; 
}


// Delete a mushroom from the game
int deleteShroom(int index){
 if(mushroomcount < 1){
   mushroomcount = 0; 
   return 0; 
 }else if(mushroomcount == 1 || mushroomcount == index+1){
 mushroomcount -= 1; 
 return 1; 
 }else{
  for (int i = index; i <= mushroomcount-1; i +=1){
    mushroomx[i]=mushroomx[i + 1]; 
    mushroomy[i]=mushroomy[i + 1]; 
 } 
mushroomcount -= 1; 
return 1;  
 }
}

// Delete a shot from the game
int deleteShot(int index){
 if(shotsFired < 1){
 shotsFired = 0; 
return 0;  
 }else if(shotsFired ==1||shotsFired == index+1){
   shotsFired-= 1; 
   return 1; 
}else{
   for (int i = index; i <= shotsFired-1; i +=1){
    shotsx[i]=shotsx[i + 1]; 
    shotsy[i]=shotsy[i + 1]; 
 } 
shotsFired -= 1; 
 return 1; 
 }
}

void loop(void) {
  buttonHooks();
  potHook();
  // if the time has come to start
  if(millis() > startTime){
  p1x = map(potRead, 0, 1023, 0, 128 - round(p1s)); // get player 1 paddle vertical position from pot on A5
    // u8g picture loop
    u8g.firstPage();
    do {
      draw();
    } while( u8g.nextPage() );
  
    startTime = millis() + (1000/14); // delay next draw to reduce framerate

  // centipede needs to u turn
    if(centix[0] <= br || centix[0] >= (120-br)|| mushroomhit(centix[0],centiy[0]) >=0){ 
        centiy[0] += 1; 
        dx =dx * -1; // switch centipede direction
    }   
    
// Adjust the centipede body to follow the head    
for (int i= centiLength; i > 0; i-=1) {
    centix[i] =centix[i-1];
    centiy[i] =centiy[i-1];
}

// The centipede has reached the bottom
    if(centiy[0] >= 63 - br){ 
    gameover = -1;
  }

// The centipede has been destroyed
if (centiLength < 1){ 
  gameover = 1; 
}


// The head of the centipede advances    
    centix[0] +=dx*v; 
    if(v >= mv){ v = mv; } // if centipede is going too fast, set velocity to max velocity

// Shots are fired
if (aIsPressed){
  if ( shotsFired < MAXSHOTS){
  shotsFired +=1;
    for (int i  =shotsFired-1; i>=1; i -=1){
    shotsx[i]= shotsx[i-1];
    shotsy[i]= shotsy[i-1];
    }
  shotsx[0]=p1x;
  shotsy[0]=p1y;
  }
delay(200); 
}

if (bIsPressed) { 
reset(); 
delay(200); 
}

// Check the shots for mushroom collisions 
temp = 0; 
for (int i  =  0; i< shotsFired; i +=1){
  // Mushroom hit
  temp = mushroomhit(shotsx[i], shotsy[i]); 
    if (temp >= 0){
    deleteShot(i); 
    p1score +=1000; 
    deleteShroom(temp); 
    }
}

// Centipede hit
temp = 0; 
for (int i  =  0; i< shotsFired; i +=1){
  temp = centihit(shotsx[i], shotsy[i]); 
    if (temp){
    p1score +=3000; 
    deleteShot(i);    
  }
}
    
// The shot reaches the top it is deleted
for (int i  =  0; i< shotsFired; i +=1){    
    if ( shotsy[i]<2){
    deleteShot(i);  
  }
}


// Move the shots up one pixel
for (int i  =  0; i< shotsFired; i +=1){
  shotsy[i]= shotsy[i]-1;
}



    if(pauseAfter > 0 && millis() > pauseAfter){ // if pauseAfter is set, and we have reached the time to pause after
      startTime = millis() + 1000; // delay program for 1000ms
      pauseAfter = 0; // reset pauseAfter
    }
  }
}