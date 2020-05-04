#ifndef BUTTONLISTENER_H
#define BUTTONLISTENER_H

#include <Arduino.h>

class ButtonListener {
  private:
    int wKey;
    int aKey;
    int xKey;
    int dKey;
    int fKey;

  public:
    ButtonListener(int inWKey, int inAKey, int inXKey, int inDKey, int inFKey);
    
    void takeInput(char *movement, byte *fire);
};

#endif
