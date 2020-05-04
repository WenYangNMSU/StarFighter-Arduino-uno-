#define LINE_PER_PAGE 16
#define CHAR_PER_LINE 15
#define FRAME_RATE 1

#define READY_KEY 'r'
#define STARTING_LIFE 3
#define SEED 80219991

#define W_PIN 8
#define X_PIN 7
#define A_PIN 12
#define D_PIN 4
#define F_PIN 19

#include "TimedAction.h"
#include "GameData.h"
#include "BackgroundMusic.h"
#include "ButtonListener.h"

// https://isocpp.org/wiki/faq/pointers-to-members
// member function pointer
// pointer to game data
GameData *gameDataP;
BackgroundMusic *backgroundMusicP;

// wrapper for gamedata update
void gameDataUpdateWrapper() {
  gameDataP -> updateGrid();
}
void musicUpdateWrapper() {
  backgroundMusicP -> playNode();
}

TimedAction gameUpdate = TimedAction(1000 / FRAME_RATE, gameDataUpdateWrapper);
TimedAction musicUpdate = TimedAction(125, musicUpdateWrapper);

void setup() {
  Serial.begin(115200);
  debug();
//  keyboardMode();
//  start();
//  buttonTest();
}
void loop() {}

/**
 * Entry point of game, called at the start of the program
 */
void start() {
  char inputValue;

  // print 1 frame worth of empty lines to setup screen, first frame of the game.
  // used to setup the screen
  for (int i = 0; i < LINE_PER_PAGE + 1; i++) {
    Serial.println("");
  }

  // correct the screen location if the screen doesn't match where it should be.
  // Second frame
  Serial.println("Calibrating your monitor.");
  Serial.print("Click clear output, type ");
  Serial.print(READY_KEY);
  Serial.println(" when you are ready");
  
  for (int i = 0; i < LINE_PER_PAGE - 2; i++) {
    Serial.println("");
  }

  // check for input
  while(true) {
    while(!Serial.available());
    inputValue = Serial.read();
    if (inputValue == READY_KEY) {
      break;
    }
  }

  // go to count down screen
  startScreen();
}

/**
 * This is the count down before game starts
 */
void startScreen() {
  // game should return back to here when game over.
  while (true) {
    char inputValue;
    // third frame, start screen
  
    // (LINE_PER_PAGE / 3) lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 3) + 1) lines left
    for (int i = 0; i < LINE_PER_PAGE / 3; i++) {
      Serial.println("");
    }
  
    // 3 lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2) lines left
    int titleCenterSpace = getCenterSpace(13);
    for (int i = 0; i < titleCenterSpace; i++) {
      Serial.print(" ");
    }
    Serial.print("Star Fighters\n");
    Serial.println("");
    
    int instrCenterSpace = getCenterSpace(21);
    for (int i = 0; i < instrCenterSpace; i++) {
      Serial.print(" ");
    }
    Serial.print("Press ");
    Serial.print(READY_KEY);
    Serial.print(" to start game\n");
  
    // (LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2) lines here
    for (int i = 0; i < LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2; i++) {
      Serial.println("");
    }
    
    // if we decided to add a settings section, this will be the place for that. 
    // check for input
    while(true) {
      while(!Serial.available());
      inputValue = Serial.read();
      if (inputValue == READY_KEY) {
        // when player is ready, go to count down screen
        countDownScreen();
        break;
      }
    }
  }
}

/**
 * This is the count down screen before the game actually starts. 
 */
void countDownScreen() {

  // used for converting int to string.
  char bufferChar[1];
  
  // count down from 3 to 0
  for (int i = 3; i >= 0; i--) {
    
    // (LINE_PER_PAGE / 2) lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 2) + 1) lines left
    for (int i = 0; i < LINE_PER_PAGE / 2; i++) {
      Serial.println("");
    }
    
    // 1 lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 2)) lines left
    
    int countDownCenterSpace = (i != 0) ? getCenterSpace(1) : getCenterSpace(3);
    for (int i = 0; i < countDownCenterSpace; i++) {
      Serial.print(" ");
    }
    // itoa convert int to string
    Serial.print((i != 0) ? itoa(i, bufferChar, 10) : "GO!");
    
    
    // (LINE_PER_PAGE - (LINE_PER_PAGE / 2)) lines here
    for (int i = 0; i < LINE_PER_PAGE - (LINE_PER_PAGE / 2); i++) {
      Serial.println("");
    }
    delay(1000);
  }
  game();
  
}

/**
 * The actual game content
 */
void game() {
  int score;
  
  // data initiation
  GameData gameData(STARTING_LIFE, LINE_PER_PAGE, CHAR_PER_LINE, SEED);
  gameDataP = &gameData;
  
  // song initation
  BackgroundMusic backgroundMusic;
  backgroundMusicP = &backgroundMusic;
  
  // input initiation
  ButtonListener buttonListener(W_PIN, A_PIN, X_PIN, D_PIN, F_PIN);
  char movement = 's';
  char fire;
  
  // frame update
  while (true) {
    gameUpdate.check();
    musicUpdate.check();
    
    buttonListener.takeInput(&movement, &fire);
    if (fire) {
      gameData.armPlayer();
      fire = false;
    }
    if (movement != 's') {
      gameData.setPlayerDirection(movement);
      movement = 's';
    }

    if (gameData.quit) {
      score = gameData.getScore();
      noTone(5);
      gameData.quitGame();
      break;
    }
  }
  
  // print score screen.
  gameOverScreen(score);
  
}

/**
 * Game over screen.
 * it should wait for a input, then return to main screen.
 */
void gameOverScreen(int score) {
  char inputValue;

  // (LINE_PER_PAGE / 3) lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 3) + 1) lines left
  for (int i = 0; i < LINE_PER_PAGE / 3; i++) {
    Serial.println("");
  }

  // 3 lines here, (LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2) lines left
  int titleCenterSpace = getCenterSpace(9);
  for (int i = 0; i < titleCenterSpace; i++) {
    Serial.print(" ");
  }
  Serial.print("Game Over\n");

  for (int i = 0; i < getCenterSpace(String("Score: " + score).length()); i++) {
    Serial.print(" ");
  }
  Serial.print("Score: ");
  Serial.println(score);
  
  int instrCenterSpace = getCenterSpace(25);
  for (int i = 0; i < instrCenterSpace; i++) {
    Serial.print(" ");
  }
  Serial.print("Press ");
  Serial.print(READY_KEY);
  Serial.print(" to return to main Screen\n");

  // (LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2) lines here
  for (int i = 0; i < LINE_PER_PAGE - (LINE_PER_PAGE / 3) - 2; i++) {
    Serial.println("");
  }
  
  // check for input
  while(true) {
    while(!Serial.available());
    inputValue = Serial.read();
    if (inputValue == READY_KEY) {
      return;
    }
  }
  
}

/**
 * returns the amount of space needed to center a string given its length
 */
int getCenterSpace(int stringLength) {
  return (CHAR_PER_LINE / 2 - stringLength / 2);
}

/**
 * debug page
 */
void debug() {
  while (true) {
    int debugCenterSpace = getCenterSpace(10);
    for (int i = 0; i < debugCenterSpace; i++) {
      Serial.print(" ");
    }
    Serial.println("Debug mode");
    Serial.println("enter n for next frame.");
    Serial.println("w, a, x, d, q, e, z, c for move");
    Serial.println("f for fire");
    Serial.println("r, t, y prints position, direction, and magnitute respectively");
    for (int i = 0; i < (LINE_PER_PAGE - 5); i++) {
      Serial.println("");
    }
    
    char input;
    GameData gameData(3, LINE_PER_PAGE, CHAR_PER_LINE);
    while(true) {
      while(!Serial.available());
      input = Serial.read();
      
      if (input == 'n') {
        gameData.updateGrid();
      }
      
      if (input == 'w' || input == 'a' || input == 'x' || input == 'd' || input == 'q'|| input == 'e'|| input == 'z'|| input == 'c') {
        gameData.setPlayerDirection(input);
      }
  
      if (input == 'f') {
        gameData.armPlayer();
      }
      
      if (input == 'r') {
        gameData.printPosition();
      }
      if (input == 't') {
        gameData.printDirection();
      }
      if (input == 'y') {
        gameData.printMagnitude();
      }
  
      if (gameData.quit) {
        int score = gameData.getScore();
        gameData.quitGame();
        gameOverScreen(score);
        break;
      }
    }
  }
}

/**
 * keyboard mode.
 */
void keyboardMode() {
  while (true) {
    int debugCenterSpace = getCenterSpace(10);
    for (int i = 0; i < debugCenterSpace; i++) {
      Serial.print(" ");
    }
    Serial.println("Keyboard mode");
    Serial.println("enter n for next frame.");
    Serial.println("w, a, s, d for move");
    Serial.println("f for fire");
    
    for (int i = 0; i < (LINE_PER_PAGE - 4); i++) {
      Serial.println("");
    }
    
    char input;
    GameData gameData(STARTING_LIFE, LINE_PER_PAGE, CHAR_PER_LINE);
    while(true) {
      while(!Serial.available());
      input = Serial.read();
      
      if (input == 'w' || input == 'a' || input == 'x' || input == 'd') {
        gameData.setPlayerDirection(input);
      }
  
      if (input == 'f') {
        gameData.armPlayer();
      }

      if (input == 's') {
        gameData.setPlayerDirection('x');
      }
  
      if (gameData.quit) {
        int score = gameData.getScore();
        gameData.quitGame();
        gameOverScreen(score);
        break;
      }

      gameData.updateGrid();
    }
  }
}

/**
 * test for buttons
 */
void buttonTest() {
  ButtonListener buttonListener(W_PIN, A_PIN, X_PIN, D_PIN, F_PIN);
  char movement;
  char fire;

  while (true) {
    while(!Serial.available());
    Serial.read();
    buttonListener.takeInput(&movement, &fire);

    Serial.print(movement);
    Serial.print(" ");
    Serial.println(fire);
  }
}
