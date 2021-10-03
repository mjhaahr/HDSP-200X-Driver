/***** HDSP-200x Driver Example***** 
Scrolling Dot Test
 - Draws a dot the runs along each row of each character then repeats

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
  scrollingDot();
  delay(5);
}

void scrollingDot(void) {
  // draw one pixel as it goes
  byte pixel = 0;
  for (int i = 0; i < 4; i++) {  // i is char number
    for (int j = 0; j < 7; j++) {  // j is row number 
      unsigned long out = 0x08000000; // 1 at top of used bit stack
      out = out >> pixel;
      pixel++;
      display.writeData(out);
      for (int k = 0; k < 5; k++) { // k is column number
        // directly addressing columns for void character control
        digitalWrite(column[k], HIGH); 
        delayMicroseconds(2500); 
        digitalWrite(column[k], LOW); 
        delayMicroseconds(20000); 
      }
    }
  }
}
