/***************************************************
DFPlayer - A Mini MP3 Player For Arduino
 <https://www.dfrobot.com/product-1121.html>

 ***************************************************
 This example shows the basic function of library for DFPlayer.

 Created 2016-12-07
 By [Angelo qiao](Angelo.qiao@dfrobot.com)

 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/****************************************************
  Libraries
*****************************************************/
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include "pitches.h"
#include <ShiftRegister74HC595.h>

/****************************************************
  Variables
*****************************************************/
SoftwareSerial softSerial(/*rx =*/12, /*tx =*/13); //Makes any pins a serial port
#define FPSerial softSerial //Define the serial port for the DFPlayer
DFRobotDFPlayerMini myDFPlayer;  //Create the DFPlayer object
const int DS_pin = A0;    // Data pin 
const int STCP_pin = A1;  // Latch pin 
const int SHCP_pin = A2;  // Clock pin 

#define speakerPin 11
const uint8_t buttonPins[] = {2, 3, 4, 5, 6, 7, 8}; //Array of button pins

const uint8_t digits[] = {
  0b00111111,  // 0
  0b00000110,  // 1
  0b01011011,  // 2
  0b01001111,  // 3
  0b01100110,  // 4
  0b01101101,  // 5
  0b01111101,  // 6
  0b00000111,  // 7
  0b01111111,  // 8
  0b01101111   // 9
};

const uint8_t leds[] = {
  B00000001, // Q0
  B00000010, // Q1
  B00000100, // Q2
  B00001000, // Q3
  B00010000, // Q4
  B00100000, // Q5
  B01000000, // Q6
  B10000000, // Q7
  B00000000, // OFF
  B11111111, // ON
};

const uint8_t notes[] = { NOTE_A3, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_D5, NOTE_F5 };

uint8_t score = 0;
String playerId = "";
uint8_t sequence[100] = {};


/****************************************************
  Function declarations
*****************************************************/
void game();
void initialiseButtons();
void displayScore(uint8_t value);
void lightLed(uint8_t value);
void saveScore();
bool postScore();
void sendScore(String playerId, uint8_t score);
void playSequence();
boolean checkUserInput();
void resetGame();
void gameOver();
void gameWon();
String getId();
void initialiseDFPlayer();
void printDetail(uint8_t type, uint8_t value);


/****************************************************
  Setup
*****************************************************/
void setup()
{
  FPSerial.begin(9600);
  Serial.begin(115200);

  pinMode(speakerPin, OUTPUT);
  pinMode(DS_pin, OUTPUT);
  pinMode(STCP_pin, OUTPUT);
  pinMode(SHCP_pin, OUTPUT);
  initialiseButtons();
  initialiseDFPlayer();

  resetGame();
}

/****************************************************
  Loop
*****************************************************/
void loop()
{
  game();
}

/****************************************************
  Functions implementations
*****************************************************/
void game(){
  Serial.print(F("Score: "));
  Serial.println(score);
  if (score>99){ //In the low probability the score goes over 99
    gameWon();  //Play the third mp3
  }
  if (score%10==0 && score!=0){ //Play a sound every 10 points
    myDFPlayer.play(2);  //Play the second mp3
    delay(1000);
  }

  displayScore(score); //Display the score
  sequence[score] = random(0, 7); //Add a new value to the sequence
  playSequence(); //Play the sequence of the game
  if(!checkUserInput()){ //Check if the user input is correct
    gameOver(); //Game over
  }
  else {
    score++;
  }
  if (myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }
  delay(1000);
}

//Initialise the buttons
void initialiseButtons(){
  Serial.println(F("Initialise buttons"));
  for (uint8_t i = 0; i < sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

//Display the score on the 7-segment display
void displayScore(uint8_t value){
  Serial.print(F("Display score "));
  Serial.print(value);
  Serial.print(F(" --> Tens: "));
  Serial.print(digits[value%100/10]);
  Serial.print(F(" Ones: "));
  Serial.println(digits[value%10]);
  uint8_t values[2] = {digits[value%100/10], digits[value%10]}; //Get the tens and units digits of the score
  digitalWrite(STCP_pin, LOW);  // Latch pin low
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, values[1]);
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, values[0]);
  digitalWrite(STCP_pin, HIGH); // Latch pin high
}

//Light the led and make noise corresponding to the value
void lightLed(uint8_t value){
  Serial.print(F("Light led "));
  Serial.println(value);
  uint8_t values[1] = {leds[value]};
  digitalWrite(STCP_pin, LOW);  // Latch pin low
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, values[0]);
  digitalWrite(STCP_pin, HIGH); // Latch pin high
  tone(speakerPin, notes[value]);
  delay(500);
  digitalWrite(STCP_pin, LOW);  // Latch pin low
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, leds[8]);
  digitalWrite(STCP_pin, HIGH); // Latch pin high
  noTone(speakerPin);
}

//Read the button inputs
uint8_t buttonInput() {
  Serial.println(F("Waiting for input"));
  while(true){ //Reset the game by pressing the any button
    for (uint8_t i = 0; i < sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        return i;
      }
    }
  }
}

//Play the sequence of the game
void playSequence() {
  Serial.println(F("CPU turn"));
  uint8_t test = score+1;
  for (uint8_t i = 0; i < test; i++) {
    lightLed(sequence[i]);
    delay(100);
  }
}

//Check if the user input is correct
boolean checkUserInput() {
  Serial.println(F("Player's turn"));
  uint8_t test = score+1;
  for (uint8_t i = 0; i < test; i++) {
    uint8_t expected = sequence[i];
    uint8_t actual = buttonInput();
    lightLed(actual);
    if (expected != actual) {
      return false;
    }
  }
  return true;
}

//Reset the game
void resetGame(){
  Serial.println(F("Resetting game ..."));
  delay(random(0, 100));
  randomSeed(micros());
  score = 0;
  playerId = "";
  displayScore(score);
  digitalWrite(STCP_pin, LOW);  // Latch pin low
  shiftOut(DS_pin, SHCP_pin, MSBFIRST, leds[8]);
  digitalWrite(STCP_pin, HIGH); // Latch pin high
  while (playerId == "") {
    playerId = getId();
  }
}

void saveScore() {
  Serial.println(F("Saving score ..."));
  bool success = postScore();
  if (success)
  {
    Serial.println(F("Score saved"));
  }
  else
  {
    Serial.println(F("Score not saved"));
  }
}

void sendScore(String playerId, uint8_t score)
{
  Serial.println("data|" + playerId + "|" + score);
}

bool postScore()
{
  sendScore(playerId, score);
  while (!Serial.available())
  {
    // wait for input
  }
  String result = Serial.readString();
  return result == "ok";
}

//Game over
void gameOver(){
  Serial.println(F("Game Over!"));
  myDFPlayer.play(1);
  saveScore();
  buttonInput();
  resetGame();
}

//Game won
void gameWon(){
  Serial.println(F("You won!"));
  myDFPlayer.play(3);
  saveScore();
  buttonInput();
  resetGame();
}

String getId() {
  while (!Serial.available()) {}
  String data = Serial.readStringUntil('\n');
  if (data.indexOf("message") != -1) {
    String message = data.substring(data.indexOf('|') + 1);
    Serial.println(message);
    return message;
  } else {
    return "";
  }
}

/***************************************************
 Code below sampled from the sample code:
 <https://www.dfrobot.com/product-1121.html>

 ***************************************************/
void initialiseDFPlayer(){
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); 
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.volume(10);  //Set volume value. From 0 to 30
}

void printDetail(uint8_t type, uint8_t value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}