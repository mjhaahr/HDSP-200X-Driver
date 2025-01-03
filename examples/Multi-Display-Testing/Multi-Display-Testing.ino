/***** HDSP-200x Driver Example*****
Multi Display Test
 - Draws longer strings to multiple displays (uses 4)

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

uint8_t column[] = {2, 3, 4, 5, 6};
uint8_t data = 7;
uint8_t clock = 8;

static const uint32_t timespacing = 1500;
static uint32_t last = 0;

HDSP_200X display = HDSP_200X(column, data, clock, 4);

void setup() {}

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
