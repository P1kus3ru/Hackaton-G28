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

//Libraries
#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include "pitches.h"
#include <ShiftRegister74HC595.h>

//Variables
SoftwareSerial softSerial(/*rx =*/12, /*tx =*/13); //Makes any pins a serial port
#define FPSerial softSerial //Define the serial port for the DFPlayer
DFRobotDFPlayerMini myDFPlayer;  //Create the DFPlayer object
ShiftRegister74HC595<2> srDisplay(A0, A1, A2); //Shiftregister pins for 7-segment display {LATCH, DATA, CLOCK}
ShiftRegister74HC595<1> srLed(A3, A4, A5); //Shiftregister pins for leds {LATCH, DATA, CLOCK}

#define speakerPin 11
uint8_t score = 0;
bool gameOver = false;

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

//Function declarations
void printDetail(uint8_t type, int value);
void displayScore(ShiftRegister74HC595<2> &sr, const byte digits[], uint8_t value);
void initialiseButtons();
void initialiseDFPlayer();

//Setup
void setup()
{
  FPSerial.begin(9600);
  Serial.begin(115200);

  initialiseButtons();
  displayScore(srDisplay, digits, 0); //Initialise the display to show 0
  initialiseDFPlayer();
}

//Loop
void loop()
{
  if (!gameOver){
    if (score%10==0 && score!=0){ //Play a sound every 10 points
      myDFPlayer.play(2);  //Play the second mp3
    }
    if (score>50){ //Temporary condition to end the game
      gameOver = true;
      myDFPlayer.play(1);  //Play the second mp3
    }
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }
    score++; //Increment the score temporarily
    displayScore(srDisplay, digits, score); //Display the score
    delay(1000);
  }
}

//Functions implementation
void displayScore(ShiftRegister74HC595<2> &sr, const byte digits[], uint8_t value){
  byte values[2] = {digits[value%100/10], digits[value%10]}; //Get the tens and units digits of the score
  sr.setAll(values); //Set the display to show the score
}

void initialiseButtons(){
  for (byte i = 0; i < sizeof(buttonPins)/sizeof(buttonPins[0]); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
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
