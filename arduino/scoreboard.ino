#include <ezButton.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 4
#define CS_PIN 15

#define GAMEPAUSED 0
#define GAMESTART 1
#define GAMEON 2
#define GAMEFINISHED 3
#define WAITINGFORINPUT 4

#define GAME 0
#define SET 1
#define MATCH 2

ezButton buttonRed(4);
ezButton buttonBlue(5);

MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

int scoreBlueGame;
int scoreBlueSet;
int scoreBlueMatch;
int scoreRedGame;
int scoreRedSet;
int scoreRedMatch;

uint32_t lastMillisButtonPress;

int status = GAMEPAUSED;

String score = "";

void setup() {
  Serial.begin(9600); 

  //set buttons
  buttonRed.setDebounceTime(100);
  buttonBlue.setDebounceTime(100);

	// Intialize the object
	myDisplay.begin();

	// Set the intensity (brightness) of the display (0-15)
	myDisplay.setIntensity(0);

	// Clear the display
	myDisplay.displayClear();

  //myDisplay.displayScroll("Hello", PA_CENTER, PA_SCROLL_LEFT, 100);
  
  //start game
  startGame();

}

void startGame() {
  scoreBlueGame = 0;
  scoreBlueSet = 0;
  scoreBlueMatch = 0;
  scoreRedGame = 0;
  scoreRedSet = 0;
  scoreRedMatch = 0;

  lastMillisButtonPress = millis();


  //menu choose:
  //ProSet&GP; 3Set&GP; ProSet&AA; 3Set&AA
}

void loop() {

  buttonRed.loop();
  buttonBlue.loop();

  //if (myDisplay.displayAnimate()) {
	//  myDisplay.displayReset();
	//}

  uint32_t currentMillis = millis();  
  

  //if (myDisplay.displayAnimate()) {
	//	myDisplay.displayReset();
	//}

  //if both buttons short press
  //undo

  //if both buttons long press
  //startGame();

  //point scored
  if (buttonBlue.isPressed()) { 
    Serial.println("button blue pressed");
    //timestamp for key pressed 
    lastMillisButtonPress = millis();
    
    scoreBlueGame++;
    displayScore(GAME);

    //did blue won the game?
    if (scoreBlueSet == 6 && scoreRedSet == 6) {
      //we are playing tiebreak
      if (scoreBlueGame >= 7 && scoreBlueGame - scoreRedGame >= 2) {
        //tiebreak won
        scoreBlueSet++;
        displayScore(SET); 
        scoreBlueGame = 0;  
        scoreRedGame = 0;
      }

    } else if (scoreBlueGame >= 4 && scoreBlueGame - scoreRedGame >= 2) {  
      //todo: change if for GP possibility
      Serial.println("game won by blue");
      scoreBlueSet++;
      displayScore(SET);
      scoreBlueGame = 0;  
      scoreRedGame = 0;
    }

    //did blue won the set?
    if ((scoreBlueSet == 6 && scoreBlueSet - scoreRedSet >= 2) || scoreBlueSet == 7){ 
      Serial.println("set won by blue"); 
      scoreBlueMatch++;
      displayScore(MATCH); 
      scoreRedSet = 0;
      scoreBlueSet = 0;
    }
    //did red won the match?
    if (scoreBlueMatch > 1) { 
      Serial.println("match won by blue"); 
      displayScore(99); 
      startGame();
      //gameover: Blue wins;
    }

  } 
  if (buttonRed.isPressed()) { 
    Serial.println("button red pressed");
    //timestamp for key pressed 
    lastMillisButtonPress = millis();
    scoreRedGame++;
    displayScore(GAME);

    //did red won the game?
    if (scoreBlueSet == 6 && scoreRedSet == 6) {
      Serial.println("tiebreak");
      //we are playing tiebreak
      if (scoreRedGame >= 7 && scoreRedGame - scoreBlueGame >= 2) {
        Serial.println("tiebreak won by red");
        //tiebreak won
        scoreRedSet++;
        displayScore(SET);
        scoreBlueGame = 0;  
        scoreRedGame = 0;
      }

    } else if (scoreRedGame >= 4 && scoreRedGame - scoreBlueGame >= 2) {  //todo: change if for GP possibility
      Serial.println("game won by red");
      scoreRedSet++;
      displayScore(SET);
      scoreBlueGame = 0;
      scoreRedGame = 0;
    }
    //did red won the set?
    if ((scoreRedSet == 6 && scoreRedSet - scoreBlueSet >= 2) || scoreRedSet == 7){ 
      Serial.println("set won by red"); 
      scoreRedMatch++;
      displayScore(MATCH);
      scoreRedSet = 0;
      scoreBlueSet = 0;
    }

    //did red won the match?
    if (scoreRedMatch > 1) { 
      Serial.println("match won by red");
      displayScore(99); 
      startGame();
      //gameover: red wins;
    }
  
  }

  if (currentMillis > (lastMillisButtonPress + (10*1000))) {
    displayScore(99); 
  }
}

void displayScore(int type) {

  String currentScore="";
  String scoreBlueGameString = "";
  String scoreRedGameString = "";

  switch (type) { 

    case GAME:
      if (scoreBlueSet == 6 && scoreRedSet == 6) { //or proset 
        //don't translate, it's a tiebreak
        scoreBlueGameString = String(scoreBlueGame);
        scoreRedGameString = String(scoreRedGame);    
      } else if ((scoreBlueGame >= 4 || scoreRedGame >= 4) && scoreBlueGame != scoreRedGame) {
        //we are in advantages
        if (scoreBlueGame > scoreRedGame) {
          scoreBlueGameString = "A";
          scoreRedGameString = "";
        } else if (scoreRedGame > scoreBlueGame) {
          scoreRedGameString = "A";
          scoreBlueGameString = "";
        } 
      } else {
        switch (scoreBlueGame) {
          case 0: scoreBlueGameString = "0"; break;
          case 1: scoreBlueGameString = "15"; break;
          case 2: scoreBlueGameString = "30"; break;
          default: scoreBlueGameString = "40";
        }
        switch (scoreRedGame) {
          case 0: scoreRedGameString = "0"; break;
          case 1: scoreRedGameString = "15"; break;
          case 2: scoreRedGameString = "30"; break;
          default: scoreRedGameString = "40";
        }
      }

      currentScore = scoreBlueGameString + "-" + scoreRedGameString;
      break;

    case SET: 

      currentScore = "s:" + String(scoreBlueSet) + "-" + String(scoreRedSet);
      break;

    case MATCH:
    
      currentScore = "m:" + String(scoreBlueMatch) + "-" + String(scoreRedMatch);
      break;

    default:
      currentScore = scoreBlueMatch;
      currentScore += "-";
      currentScore += scoreRedMatch;
      currentScore += ".";
      currentScore += scoreBlueSet;
      currentScore += "-";
      currentScore += scoreRedSet;

      //myDisplay.displayClear();

      

  }


  myDisplay.print(currentScore.c_str());

  Serial.print("game:");
  Serial.print(scoreBlueGameString);
  Serial.print("-");
  Serial.println(scoreRedGameString);
  Serial.print("set:");
  Serial.print(scoreBlueSet);
  Serial.print("-");
  Serial.println(scoreRedSet);
  Serial.print("match:");
  Serial.print(scoreBlueMatch);
  Serial.print("-");
  Serial.println(scoreRedMatch);
  
}
