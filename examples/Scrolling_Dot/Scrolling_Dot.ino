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
  display.testDisplay();
  delay(5);
}
