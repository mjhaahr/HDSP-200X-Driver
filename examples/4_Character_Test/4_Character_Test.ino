/***** HDSP-200x Drive Example***** 
4 Character Test

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

char column[] = {10,9,8,7,6};
char data = 12;
char clock = 11;

static const unsigned long timespacing = 250;
static unsigned long last = 0;

HDSP_200X display = HDSP_200X(column, data, clock);

void setup() {
}

void loop() {
  for (volatile int i = 32; i < 127; i++) {
    last = millis();
    while (millis() < last + timespacing) {
      charTesting(i);
      delay(5);
    }
  }
}

void charTesting(unsigned char letter) {
  unsigned char chars[4];
  chars[0] = letter;
  for (int i = 1; i < 4; i++) {
    unsigned char temp = letter - i;
    if (temp < 32) {
      unsigned char offset = 32 - temp;
      temp = 127 - offset;
    }
    chars[i] = temp;
  }
  
  display.writeFourChars(chars);
}
