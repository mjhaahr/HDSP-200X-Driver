/***** HDSP-200x Driver Example*****
Scrolling Dot Test
 - Draws a dot the runs along each row of each character then repeats

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

uint8_t column[] = {10,9,8,7,6};
uint8_t data = 12;
uint8_t clock = 11;

HDSP_200X display = HDSP_200X(column, data, clock, 1);

void setup() {}

void loop() {
    display.testDisplay();
    delay(5);
}
