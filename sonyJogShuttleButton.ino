 
const int ledPin =  13;      // the number of the LED pin
const uint8_t NO_KNOB = 0xF8;
const uint8_t ANY_KNOB = 0x7;
const uint8_t NEGATIVE_KNOB = 0x80;
const uint8_t STOP_PUSHED = 0x40;
const uint8_t PLAY_PUSHED = 0x20; 
const boolean DEBUG = true;
const boolean VERBOSE = false;

enum eMask {GRAY_SPEED_2 = 1, NEGATIVE = 2, PLAY = 4, STOP = 8, GRAY_SPEED_0 = 16, GRAY_SPEED_1 = 32, ALWAYS = 128};    
class Button {
  public:
    const int pin;
    const eMask mask;
    Button (int initPinNumber, eMask initMask): pin(initPinNumber),mask(initMask) {
    };
    int buttonState = 0;
    int lastButtonState = 0;
    long lastDebounceTime = 0;
    static const long debounceDelay = 100;
} stopPin(A5,STOP), playPin(A4, PLAY), fastPin(A3, GRAY_SPEED_2), NEGATIVEPin(A2, NEGATIVE), GRAY_SPEED_0Pin(A1, GRAY_SPEED_0), medium(A0,GRAY_SPEED_1);

Button button[] = {stopPin, playPin, fastPin, NEGATIVEPin, GRAY_SPEED_0Pin, medium};
const int numButtons = 6;
eMask prevState = ALWAYS, currState = ALWAYS;

void printMask(eMask mask){
  uint8_t state=0;

  
  if ((mask & GRAY_SPEED_2) && (mask & GRAY_SPEED_1) && (mask & GRAY_SPEED_0)) state |= 5;
  else if ((mask & GRAY_SPEED_2) && (mask & GRAY_SPEED_1)) state |= 4;
  else if ((mask & GRAY_SPEED_2) && (mask & GRAY_SPEED_0)) state |= 6;
  else if (mask & GRAY_SPEED_2) state |= 7;
  else if ((mask & GRAY_SPEED_0) && (mask & GRAY_SPEED_1)) state |= 2;
  else if (mask & GRAY_SPEED_1) state |= 3;
  else if (mask & GRAY_SPEED_0) state |= 1;

  //negatives please, but no negative zeros thank you
  if ((mask & NEGATIVE)&&(mask & ANY_KNOB)) state |= NEGATIVE_KNOB;

  if (mask & PLAY) state |= PLAY_PUSHED;
  if (mask & STOP) state |= STOP_PUSHED;

  if (DEBUG){
    if (VERBOSE){
      Serial.print("State: ");Serial.println(state);
    }
    if (state & NEGATIVE_KNOB) Serial.print("-");
    if (state & ANY_KNOB) Serial.print(state & ANY_KNOB);
    else Serial.print("0");
    Serial.print(" ");
    if (state == PLAY_PUSHED) Serial.print("Play");
    if (state == STOP_PUSHED) Serial.print("Stop");
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println(__TIME__);
    for (int i = 0; i < numButtons; i++) {
    Serial.print("Setting pin to input: "); Serial.println(button[i].pin);
    pinMode(button[i].pin, INPUT);
    pinMode(ledPin, OUTPUT);
  }
}

void loop() {
  currState = ALWAYS;
  for (int i = 0; i < numButtons; i++) {
    int reading = digitalRead(button[i].pin);
    //have things changed?
    if (reading != button[i].lastButtonState) {
      //reset the debouncing timer
      button[i].lastDebounceTime = millis();
    }
    if ((millis() - button[i].lastDebounceTime) > button[i].debounceDelay) {
      // has the reading been different longer than the debounceDelay time
      button[i].buttonState = reading;
    }
    // save the reading.  it'll be the lastButtonState:
    button[i].lastButtonState = reading;
    //Add flag to mask if it's on
    currState |= (button[i].mask * button[i].buttonState);
  }
  if (currState != prevState){
    printMask(currState);
    prevState = currState;
  }
}
