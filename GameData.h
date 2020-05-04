#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <Arduino.h>

class GameData {
  private:
    // all grids uses y,x for ease of printing
    
    // game grids
    // using pointer since array need size declearation
    char *positionGrid;
    char *directionGrid;
    byte *magnitudeGrid;
    byte gridSize[2];

    // game stats
    byte life;
    int score;
    byte playerPosition[2];
    byte playerArmed;

    // game sprite
    char player = 'A';
    char playerBullet = '^';

    char enermy1 = 'T';
    char enermy1Bullet = '.';

    // settings
    int enermyScore = 100;
    int enermyBulletScore = 10;
    byte enermySpawnChance = 20;
    byte enermyFireChance = 5;
    
    byte linePerPage;
    byte charPerLine;

    // support functions
    void initiateGrids();
    void moveCell(byte y, byte x);
    void spawnCell(byte y, byte x);
    bool outOfBound(byte y, byte x);
    void cleanCell(byte y, byte x);
    void getNewCoord(byte *newY, byte *newX, byte y, byte x, char dirc, int i);
    void assignCell(byte y, byte x);
    void increaseScore(char unit1, char unit2);
    
  public: 
    byte quit;
    
    GameData(byte life, byte inputLinePerPage, byte inputCharPerLine);
    GameData(byte startLife, byte inputLinePerPage, byte inputCharPerLine, unsigned long seed);
    GameData();

    void printPosition();
    void printDirection();
    void printMagnitude();
    
    int getScore();
    
    void updateGrid();
    void setPlayerDirection(char dirc);
    void armPlayer();
    void quitGame();
    
};

#endif
