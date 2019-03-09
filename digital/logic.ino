//determine how big the array up above is, by checking the size
#define NUMBUTTONS 4 //S, L, H, L+H
#define DEBOUNCE 5  // how many ms to debounce, 5+ ms is usually plenty
 
//define the buttons that we'll use.
byte buttons[] = {4, 5, 6}; // physical buttons CONFIRM, UP, DOWN
char bNames[] ="CUDJ"; // buttons names CONFIRM, UP,  DOWN, UP+DOWN
char bEvents[] ="pslr"; // event names PRESS, RELEASE AFTER SHORT, BEGIN LONG, RELEASE AFTER LONG
int longStart = 500; // delay before considered long press
 
//track if a button is just pressed, just released, or 'currently pressed' for S, L, H, L+H
byte pressed[NUMBUTTONS], oldPressed[NUMBUTTONS];
byte justPressed[NUMBUTTONS], justReleased[NUMBUTTONS];
byte longPress[NUMBUTTONS], oldLongPress[NUMBUTTONS];
byte previousKeyState[NUMBUTTONS], currentKeyState[NUMBUTTONS];

void setup() {
  byte i;
  Serial.begin(9600); //set up serial port
  Serial.print("Button checker with ");
  Serial.print(NUMBUTTONS, DEC);
  Serial.println(" buttons");
  // Make input & enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }
}

void loop() {
  char event[2] = "";
  if (checkEvent(event)) {
    Serial.print(event[0]);
    Serial.println(event[1]);
  }
}
 
void check_switches() {
  static byte previousState[NUMBUTTONS];
  static byte currentState[NUMBUTTONS];
  static long lastTime;
  static long stateSince[NUMBUTTONS];
  byte index;
  if (millis() < lastTime) {
    // we wrapped around, lets just try again
    lastTime = millis();
  }
  if ((lastTime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lastTime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justPressed[index] = 0;       //when we start, we clear out the "just" indicators
    justReleased[index] = 0;
    if (index == 3) {
      currentState[index] = digitalRead(buttons[1]) || digitalRead(buttons[2]);
    } else {
      currentState[index] = digitalRead(buttons[index]);   //read the button
    }
    if (currentState[index] == previousState[index]) {
      if ((pressed[index] == LOW) && (currentState[index] == LOW)) { //Watch out here LOW has opposite meanings
        // just pressed
        justPressed[index] = 1;
        stateSince[index] = millis();
      }
      else if ((pressed[index] == HIGH) && (currentState[index] == HIGH)) {
        justReleased[index] = 1; // just released
        stateSince[index] = millis();
      }
      pressed[index] = !currentState[index];  //remember, digital HIGH means NOT pressed
      if (millis() - stateSince[index] > longStart) {
        longPress[index] = pressed[index];
      }else{
        longPress[index] = 0;
      }
    }
    previousState[index] = currentState[index]; //keep a running tally of the buttons
  }

 boolean checkEvent(char event[2]) {
  boolean isEvent = false;
  checkSwitches();
  for (int i=3; i>=0; i--) { 
    if (longPress[i]!=oldLongPress[i]) {
      if (longPress[i]==1) {// start long
        event[0]=bEvents[2];
        event[1]=bNames[i];
        isEvent = true;
        break;
      }
      if (longPress[i]==0) {// released long
        event[0]=bEvents[3];
        event[1]=bNames[i];
        isEvent = true;
        break;
      }
    }
    if (pressed[i]!=oldPressed[i]) {
      if (pressed[i]==1) {// pressed
        event[0]=bEvents[0];
        event[1]=bNames[i];
        isEvent = true;
        break;
      }
      if (pressed[i]==0) {// released
        event[0]=bEvents[1];
        event[1]=bNames[i];
        isEvent = true;
        break;
      }
    }
  }
  for (int i=0; i<4; i++) {
    oldPressed[i]=pressed[i];
    oldLongPress[i]=longPress[i];
  }
  return isEvent;
}
