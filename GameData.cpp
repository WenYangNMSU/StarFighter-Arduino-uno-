#include "GameData.h"
// in the char direction grid.
// s is no movement, w is up, a is left, d is right, x is down. q, e, z, and c are movement in relation to s. 


// default constructor with 3 extra life, 22 lines per page, and 22 characters per line
GameData::GameData() {
  life = 3;
  
  linePerPage = 22;
  charPerLine = 22;

  gridSize[0] = linePerPage - 1;
  gridSize[1] = charPerLine;
  
  initiateGrids();
}

/**
 * constructor that set starting life, and monitor settings
 */
GameData::GameData(byte startLife, byte inputLinePerPage, byte inputCharPerLine) {
  life = startLife;
  
  linePerPage = inputLinePerPage;
  charPerLine = inputCharPerLine;

  gridSize[0] = linePerPage - 1;
  gridSize[1] = charPerLine;

  initiateGrids();
}

/**
 * constructor that set starting life, and monitor settings
 * also set random seed
 */
GameData::GameData(byte startLife, byte inputLinePerPage, byte inputCharPerLine, unsigned long seed) {
  randomSeed(seed);
  life = startLife;
  
  linePerPage = inputLinePerPage;
  charPerLine = inputCharPerLine;

  gridSize[0] = linePerPage - 1;
  gridSize[1] = charPerLine;

  initiateGrids();
}

/**initaite the grid to game starting values.
 * spawn player
 * 
 */
void GameData::initiateGrids() {
  quit = false;
  score = 0;
  playerArmed = false; 

// new malloc method
// need to be freed
// prevent data from going out of scope
  positionGrid = malloc(sizeof(char[gridSize[0]][gridSize[1]]));
  directionGrid = malloc(sizeof(char[gridSize[0]][gridSize[1]]));
  magnitudeGrid = malloc(sizeof(byte[gridSize[0]][gridSize[1]]));

  // memset on grid seems to only set the first few ones.
  // initiate varible
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      *(positionGrid + i * gridSize[1] + j) = ' ';
      *(directionGrid + i * gridSize[1] + j) = 's';
      *(magnitudeGrid + i * gridSize[1] + j) = 0;
    }
  }

  // assign player position
  playerPosition[0] = gridSize[0] - 1;
  playerPosition[1] = gridSize[1] / 2;
  
  *(positionGrid + playerPosition[0] * gridSize[1] + playerPosition[1]) = player;
}

/**called every frame. 
 * Update the array according to the direction and maganitute maxtrix
 * reset magnitute matrix and set direction matrix on correct cell, 
 * spawn entities and then reassign maganitute matrix
 */
void GameData::updateGrid() {
  // if player life dropes to 0, end the game.
  if (life <= 0) {
    quit = true;
  }
  
  // game ends when quit is true
  if (quit == true) {
    return;
  }
  
  // update position and reset direction and magnitute
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      // update if the cell isn't empty.
      if (*(positionGrid + i * gridSize[1] + j) != ' ') {
        moveCell(i, j);
      }
    }
  }

  
  // assign new values to magnitute and spawn new entities
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      // spawn new entity and assign position values
      if (*(positionGrid + i * gridSize[1] + j) != ' ' || i == 0) {
        // spawn logic
        spawnCell(i, j);

        // assign a new move direction and magnitude for the units
        if (*(positionGrid + i * gridSize[1] + j) != ' ') {
          assignCell(i, j);
        }
      }
    }
  }

  // print out the table.
  printPosition();
}

/**
 * Spawns new characters every frame.
 */
void GameData::spawnCell(byte y, byte x) {
  // if the cell is on the first row, there is a chance for it to spawn a new enermy ship
  if (y == 0) {
    if (!random(enermySpawnChance)) {
      // if the player is on the cell, just subtract 1 life from player and add the enermy score
      if (*(positionGrid + y * gridSize[1] + x) == player) {
        life--;
        increaseScore(enermy1, enermy1);
        return;
      }
      *(positionGrid + y * gridSize[1] + x) = enermy1;
    }
    return;
  }

  // if the cell is enermy, this is not the last row, there is a chance to spawn a new enermy bullet
  if (*(positionGrid + y * gridSize[1] + x) == enermy1 && y != (gridSize[0] - 1)) {
    if (!random(enermyFireChance)) {
      // if the cell contains a player bullet, clear the cell and add bullet points to player.
      if(*(positionGrid + (y + 1) * gridSize[1] + x) == playerBullet) {
        cleanCell(y + 1, x);
        increaseScore(enermy1Bullet, enermy1Bullet);
      }

      // if the cell contains player, decrease life by 1 and add bullet score.
      if(*(positionGrid + (y + 1) * gridSize[1] + x) == player) {
        life--;
        increaseScore(enermy1Bullet, enermy1Bullet);
      }
       
      *(positionGrid + (y + 1) * gridSize[1] + x) = enermy1Bullet;
      assignCell(y + 1, x);
    }
    return;
  }
  
  // if the cell is player, this is not the first row, and the player is armed. a player bullet is spawned
  if (*(positionGrid + y * gridSize[1] + x) == player && y != 0 && playerArmed) {
    playerArmed = false;
    
    // if an enermy is on the cell above, destory the enermy and don't spawn bullet
    if (*(positionGrid + (y - 1) * gridSize[1] + x) == enermy1 || *(positionGrid + (y - 1) * gridSize[1] + x) == enermy1Bullet) {
      increaseScore(*(positionGrid + (y - 1) * gridSize[1] + x), *(positionGrid + (y - 1) * gridSize[1] + x));
      cleanCell((y - 1), x);
      return;
    }
    
    *(positionGrid + (y - 1) * gridSize[1] + x) = playerBullet;
    assignCell(y - 1, x);
    return;
  }
}

/**Reassign value to the entities.
 * Spawn new entities.
 */
void GameData::assignCell(byte y, byte x) {
  // ignore player, and empty space
  if (*(positionGrid + y * gridSize[1] + x) == player || *(positionGrid + y * gridSize[1] + x) == ' ') {
    return;
  }
  
  // enermy ships always go down by 1
  if (*(positionGrid + y * gridSize[1] + x) == enermy1) {
    *(directionGrid + y * gridSize[1] + x) = 'x';
    *(magnitudeGrid + y * gridSize[1] + x) = 1;
    return;
  }

  // enermy bullet always go down by 2
  if (*(positionGrid + y * gridSize[1] + x) == enermy1Bullet) {
    *(directionGrid + y * gridSize[1] + x) = 'x';
    *(magnitudeGrid + y * gridSize[1] + x) = 2;
    return;
  }

  // player bullet always go up by 2
  if (*(positionGrid + y * gridSize[1] + x) == playerBullet) {
    *(directionGrid + y * gridSize[1] + x) = 'w';
    *(magnitudeGrid + y * gridSize[1] + x) = 2;
    return;
  }
}

/**
 * method used to update a single cell 
 * and set correct direction and 0 to magnitute
 */
void GameData::moveCell(byte y, byte x) {
  // return if no movement is needed.
  if (*(magnitudeGrid + y * gridSize[1] + x) == 0 || *(directionGrid + y * gridSize[0] + x) == 's') {
    return;
  }
  
  byte newY;
  byte newX;
  
  // check for any targets along the path
  for (int i = 1; i <= *(magnitudeGrid + y * gridSize[1] + x); i++) {
    // check for the direction
    getNewCoord(&newY, &newX, y, x, *(directionGrid + y * gridSize[1] + x), i);
    
    // check for out of bound first.
    // if it's out of bound and it's not a player, remove the character, if it's the player, don't do anything.
    if (outOfBound(newY, newX)) {
      if (*(positionGrid + y * gridSize[1] + x) != player) {
        cleanCell(y, x);
      }
      return;
    }

    // enermy charactor logic
    // if the cell contains a enermy or enermy bullet
    if (*(positionGrid + y * gridSize[1] + x) == enermy1Bullet || *(positionGrid + y * gridSize[1] + x) == enermy1) {
    
      // check if the target cell contains a player bullet.
      // if true, remove both charaters from the board.
      if (*(positionGrid + newY * gridSize[1] + newX) == playerBullet) {
        increaseScore(*(positionGrid + y * gridSize[1] + x), *(positionGrid + newY * gridSize[1] + newX));
        cleanCell(y, x);
        cleanCell(newY, newX);
        return;
      }
    
      // check if the target cell contains a player
      // if true, remove this character and subtract a life from the player.
      if (*(positionGrid + newY * gridSize[1] + newX) == player) {
        increaseScore(*(positionGrid + y * gridSize[1] + x), *(positionGrid + newY * gridSize[1] + newX));
        cleanCell(y, x);
        life--;
        return;
      }
    }

    // friendly charactor logic
    // if the cell contains a player bullet
    if (*(positionGrid + y * gridSize[1] + x) == playerBullet) {
      // check if the target cell contains a enermy or enermy bullet
      // if true, remove both characters
      if (*(positionGrid + newY * gridSize[1] + newX) == enermy1Bullet || *(positionGrid + newY * gridSize[1] + newX) == enermy1) {
        increaseScore(*(positionGrid + y * gridSize[1] + x), *(positionGrid + newY * gridSize[1] + newX));
        cleanCell(y, x);
        cleanCell(newY, newX);
        return;
      }
    }

    // if the cell contains player
    if (*(positionGrid + y * gridSize[1] + x) == player) {
      // check if the target cell contains a enermy or enermy bullet
      // if true, clean the target cell, subtracte a life from the player, and move on to moving logic
      if (*(positionGrid + newY * gridSize[1] + newX) == enermy1Bullet || *(positionGrid + newY * gridSize[1] + newX) == enermy1) {
        increaseScore(*(positionGrid + y * gridSize[1] + x), *(positionGrid + newY * gridSize[1] + newX));
        cleanCell(newY, newX);
        life--;
      }
    }
  }
  
  // move logic
  // newY and newX should now contain the correct coord.

  // if the target is not empty. ie a friendly unit.
  // first try to move the occupent character
  if (*(positionGrid + newY * gridSize[1] + newX) != ' ') {
    moveCell(newY, newX);
  }

  // if the target cell is emptied
  // just move the character.
  if (*(positionGrid + newY * gridSize[1] + newX) == ' ') {
    // if the character is player, change player position
    if (*(positionGrid + y * gridSize[1] + x) == player) {
      playerPosition[0] = newY;
      playerPosition[1] = newX;
    }
    
    *(positionGrid + newY * gridSize[1] + newX) = *(positionGrid + y * gridSize[1] + x);
    *(directionGrid + newY * gridSize[1] + newX) = 's';
    *(magnitudeGrid + newY * gridSize[1] + newX) = 0;

    
    cleanCell(y, x);
    return;
  }

  // if the cell is still not empty after moving the previous cell
  // the new cell will keep the more important unit.
  // importance list, high to low: player, player bullet, enermy, enermy bullet.
  if (*(positionGrid + y * gridSize[1] + x) == player || *(positionGrid + newY * gridSize[1] + newX) == player) {
    *(positionGrid + newY * gridSize[1] + newX) = player;
    playerPosition[0] = newY;
    playerPosition[1] = newX;
    
  } else if (*(positionGrid + y * gridSize[1] + x) == playerBullet || *(positionGrid + newY * gridSize[1] + newX) == playerBullet) {
    *(positionGrid + newY * gridSize[1] + newX) = playerBullet;
    
  } else if (*(positionGrid + y * gridSize[1] + x) == enermy1 || *(positionGrid + newY * gridSize[1] + newX) == enermy1) {
    *(positionGrid + newY * gridSize[1] + newX) = enermy1;
    
  } else if (*(positionGrid + y * gridSize[1] + x) == enermy1Bullet || *(positionGrid + newY * gridSize[1] + newX) == enermy1Bullet) {
    *(positionGrid + newY * gridSize[1] + newX) = enermy1Bullet;
  }

  *(directionGrid + newY * gridSize[1] + newX) = 's';
  *(magnitudeGrid + newY * gridSize[1] + newX) = 0;

  cleanCell(y, x);
}

/**
 * Make the player armed.
 * fire a bullet in the next frame.
 */
void GameData::armPlayer() {
  playerArmed = true;
}

/**
 * increase the score base on what unit is killed
 */
void GameData::increaseScore(char unit1, char unit2) {
  if (unit1 == enermy1 || unit2 == enermy1) {
    score += enermyScore;
  }
  if (unit1 == enermy1Bullet || unit2 == enermy1Bullet) {
    score += enermyBulletScore;
  }
}

/**
 * set the direction where the player want to move.
 * player move by 1.
 */
void GameData::setPlayerDirection(char dirc) {
  *(directionGrid + playerPosition[0] * gridSize[1] + playerPosition[1]) = dirc;
  *(magnitudeGrid + playerPosition[0] * gridSize[1] + playerPosition[1]) = 1;
}

/**
 * put the right value to newY and newX base on direction
 */
void GameData::getNewCoord(byte *newY, byte *newX, byte y, byte x, char dirc, int i) {
  switch(dirc) {
      case 'q': 
        *newY = y - i;
        *newX = x - i;
        return;
      case 'w':
        *newY = y - i;
        *newX = x;
        return;
      case 'e':
        *newY = y - i;
        *newX = x + 1;
        return;
      case 'a': 
        *newX = x - i;
        *newY = y;
        return;
      case 'd':
        *newX = x + i;
        *newY = y;
        return;
      case 'z':
        *newY = y + i;
        *newX = x - 1;
        return;
      case 'x':
        *newY = y + i;
        *newX = x;
        return;
      case 'c':
        *newY = y + i;
        *newX = x + 1;
        return;
      // if it's none of those print debug and return
      default:
        *newY = y;
        *newX = x;
        Serial.print("Errer during direction calculation at (");
        Serial.print(y);
        Serial.print(" ,");
        Serial.print(x);
        Serial.print(") with direction");
        Serial.println(dirc);
        return;
    }
}

/**
 * returns true if current x, y values are out of bound.
 */
bool GameData::outOfBound(byte y, byte x) {
  if (y >= gridSize[0] || y < 0 || x >= gridSize[1] || x < 0) {
    return true;
  }
  return false;
}

/**
 * removes the character at current cell
 */
void GameData::cleanCell(byte y, byte x) {
  *(positionGrid + y * gridSize[1] + x) = ' ';
  *(directionGrid + y * gridSize[1] + x) = 's';
  *(magnitudeGrid + y * gridSize[1] + x) = 0;
}

/**
 * prints the postions of all units on the map.
 */
void GameData::printPosition() {
  // print header
  Serial.print("Life: ");
  Serial.print(life);
  Serial.print("\t\t Score: ");
  Serial.print(score);

  Serial.print("\t\t Player position: ");
  Serial.print(playerPosition[0]);
  Serial.print(", ");
  Serial.print(playerPosition[1]);
  Serial.println();
  
  // print position table
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      Serial.print(*(positionGrid + i * gridSize[1] + j));
    }
    
    Serial.println("");
  }
}

/**
 * prints direction grid
 */
void GameData::printDirection() {
  // print header
  Serial.print("Life: ");
  Serial.print(life);
  Serial.print("\t\t Score: ");
  Serial.print(score);
  Serial.println();

  // print extra table
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      Serial.print(*(directionGrid + i * gridSize[1] + j));
    }
    Serial.println("");
  }
}

/**
 * prints magnitude grid
 */
void GameData::printMagnitude() {
  // print header
  Serial.print("Life: ");
  Serial.print(life);
  Serial.print("\t\t Score: ");
  Serial.print(score);
  Serial.println("");
  
  // print extra table
  for (byte i = 0; i < gridSize[0]; i++) {
    for (byte j = 0; j < gridSize[1]; j++) {
      Serial.print(*(magnitudeGrid + i * gridSize[1] + j));
    }
    Serial.println("");
  }
}

/**
 * returns the score
 */
int GameData::getScore() {
  return score;
}

/**
 * free memory hold by grids.
 */
void GameData::quitGame() {
  quit = true;
  free(positionGrid);
  free(directionGrid);
  free(magnitudeGrid);
}
