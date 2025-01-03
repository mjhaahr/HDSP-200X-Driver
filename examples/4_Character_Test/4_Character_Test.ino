/***** HDSP-200x Driver Example*****
4 Character Test
 - Draws 4 characters to the display
 - Goes through each character definied in the font matrix

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

uint8_t column[] = {1, 2, 3, 4, 5};
uint8_t data = 6;
uint8_t clock = 7;

static const unsigned long timespacing = 250;
static unsigned long last = 0;

HDSP_200X display = HDSP_200X(column, data, clock, 1);

void setup() {}

void loop() {
    for (uint8_t i = 0x20; i < 0x7F; i++) {
        last = millis();
        charTesting(i);
        while (millis() < last + timespacing) {
            display.clear();
            display.writeBuffer();
            delay(5);
        }
    }
}

void charTesting(unsigned char letter) {
    unsigned char chars[4];
    chars[0] = letter;
    for (uint8_t i = 1; i < 4; i++) {
        unsigned char temp = letter - i;
        if (temp < 0x20) {
            unsigned char offset = 0x20 - temp;
            temp = 0x7F - offset;
        }
        chars[i] = temp;
    }

    display.updateString(chars);
}
