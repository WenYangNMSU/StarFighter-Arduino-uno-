#include "BackgroundMusic.h"

/**
 * constructor, set the index to 0.
 */
BackgroundMusic::BackgroundMusic() {
  pinMode(5, OUTPUT);
  index = 0;
  subIndex = 0;
}

void BackgroundMusic::playNode() {
  
  if (index > songLength) {
    index = 0;
    subIndex = 0;
  }

  tone(5, melody[index]);
  
  subIndex++;
  if (subIndex >= (noteduration[index] / 125)) {
    index++;
    subIndex = 0;
  }
}
