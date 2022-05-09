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

HDSP_200X display = HDSP_200X(column, data, clock, 4);

void setup() {
}

void loop() {
  display.updateString("This is a test");
  display.draw();
  
  last = millis();
  while (millis() < last + timespacing) {
    delay(3);
  }

  display.pause();
  display.updateString("Thanks for using");
  display.draw();
  
  last = millis();
  while (millis() < last + timespacing) {
    delay(3);
  }

  display.pause();
  display.updateString("Have Fun!");
  display.draw();
  
  last = millis();
  while (millis() < last + timespacing) {
    delay(3);
  }
}
