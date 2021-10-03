/***** HDSP-200x Driver Example***** 
Multi Display Test
 - Draws longer strings to multiple displays (uses 4)

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

char column[] = {10,9,8,7,6};
char data = 12;
char clock = 11;

static const unsigned long timespacing = 1500;
static unsigned long last = 0;

HDSP_200X display = HDSP_200X(column, data, clock);

void setup() {
}

void loop() {
  last = millis();
  while (millis() < last + timespacing) {
    display.writeNChars(4, "This is a Test  ");
    delay(3);
  }
  last = millis();
  while (millis() < last + timespacing) {
    display.writeNChars(4, "Thanks for Using");
    delay(3);
  }
  last = millis();
  while (millis() < last + timespacing) {
    display.writeNChars(4, "Have Fun!       ");
    delay(3);
  }
}
