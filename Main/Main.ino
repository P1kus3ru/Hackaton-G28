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
ShiftRegister74HC595<2> srDisplay(A0, A1, A2); //Shiftregister pins for 7-segment display {LATCH, DATA, CLOCK}
ShiftRegister74HC595<1> srLed(A3, A4, A5); //Shiftregister pins for leds {LATCH, DATA, CLOCK}

#define speakerPin 11
const uint8_t buttonPins[] = {2, 3, 4, 5, 6, 7, 8}; //Array of button pins

const byte digits[] = {
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
};

const byte leds[] = {
  B00000001, // Q0
  B00000010, // Q1
  B00000100, // Q2
  B00001000, // Q3
  B00010000, // Q4
  B00100000, // Q5
  B01000000, // Q6
  B10000000, // Q7
};

const int notes[] = { NOTE_A3, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_D5, NOTE_F5 };

uint8_t score = 0;
uint8_t sequence[100] = {};


/****************************************************
  Function declarations
*****************************************************/
void game();
void initialiseButtons();
void displayScore(uint8_t value);
void lightLed(uint8_t value);
void saveScore(uint8_t score);
void playSequence();
boolean checkUserInput();
void resetGame();
void gameOver();
void gameWon();
void initialiseDFPlayer();
void printDetail(uint8_t type, int value);


/****************************************************
  Setup
*****************************************************/
void setup()
{
  FPSerial.begin(9600);
  Serial.begin(115200);

  pinMode(speakerPin, OUTPUT);
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
  for (byte i = 0; i < sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
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
  byte values[2] = {digits[value%100/10], digits[value%10]}; //Get the tens and units digits of the score
  srDisplay.setAll(values); //Set the display to show the score
}

//Light the led and make noise corresponding to the value
void lightLed(uint8_t value){
  Serial.print(F("Light led "));
  Serial.println(value);
  byte values[1] = {leds[value]};
  srLed.setAll(values); //Set the leds to show the value
  tone(speakerPin, notes[value]);
  delay(500);
  srLed.setAllLow(); //Turn off the leds
  noTone(speakerPin);
}

//Read the button inputs
uint8_t buttonInput() {
  Serial.println(F("Waiting for input"));
  while(true){ //Reset the game by pressing the any button
    for (byte i = 0; i < sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
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
  for (int i = 0; i < test; i++) {
    lightLed(sequence[i]);
    delay(100);
  }
}

//Check if the user input is correct
boolean checkUserInput() {
  Serial.println(F("Player's turn"));
  uint8_t test = score+1;
  for (uint8_t i = 0; i < test; i++) {
    byte expectedButton = sequence[i];
    byte actualButton = buttonInput();
    lightLed(actualButton);
    if (expectedButton != actualButton) {
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
  displayScore(score);
  srLed.setAllLow();
}

void saveScore(uint8_t score){
  Serial.println(F("Saving score ..."));
  //Code to save the score to Database
}

//Game over
void gameOver(){
  Serial.println(F("Game Over!"));
  myDFPlayer.play(1);
  saveScore(score);
  buttonInput();
  resetGame();
}

//Game won
void gameWon(){
  Serial.println(F("You won!"));
  myDFPlayer.play(3);
  saveScore(score);
  buttonInput();
  resetGame();
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

void printDetail(uint8_t type, int value){
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