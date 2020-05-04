#include "ButtonListener.h"

/**
 * set the input keys
 */
ButtonListener::ButtonListener(int inWKey, int inAKey, int inXKey, int inDKey, int inFKey) {
  wKey = inWKey;
  aKey = inAKey;
  xKey = inXKey;
  dKey = inDKey;
  fKey = inFKey;

  pinMode(wKey, INPUT_PULLUP);
  pinMode(aKey, INPUT_PULLUP);
  pinMode(xKey, INPUT_PULLUP);
  pinMode(dKey, INPUT_PULLUP);
  pinMode(fKey, INPUT_PULLUP);
}

/**
 * check which direction the player should move, and if the player should fire next round.
 */
void ButtonListener::takeInput(char *movement, byte *fire) {
  // currently we don't have a fire key.
  // so constant fire.
  if (digitalRead(fKey) == LOW) {
    *fire = true;
  }

  if (digitalRead(wKey) == LOW) {
    if (digitalRead(aKey) == LOW) {
      *movement = 'q';
    } else if (digitalRead(dKey) == LOW) {
      *movement = 'e';
    } else {
      *movement = 'w';
    }
    return;
  }

  if (digitalRead(xKey) == LOW) {
    if (digitalRead(aKey) == LOW) {
      *movement = 'z';
    } else if (digitalRead(dKey) == LOW) {
      *movement = 'c';
    } else {
      *movement = 'x';
    }
    return;
  }

  if (digitalRead(aKey) == LOW) {
    *movement = 'a';
    return;
  }

  if (digitalRead(dKey) == LOW) {
    *movement = 'd';
    return;
  }

  *movement = 's';
  return;
}
