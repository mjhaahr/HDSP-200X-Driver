/***** HDSP-200x Driver Example*****
4 Character Test
 - Draws 4 characters to the display
 - Goes through each character definied in the font matrix

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

uint8_t column[] = {2, 3, 4, 5, 6};
uint8_t data = 7;
uint8_t clock = 8;

static const uint32_t timespacing = 1000;
static uint32_t last = 0;

HDSP_200X display = HDSP_200X(column, data, clock, 1);

void setup() {
    Serial.begin(9600);
    display.draw();
}

void loop() {
    for (uint8_t i = 0x20; i < 0x7F; i++) {
        last = millis();
        charTesting(i);
        while (millis() < last + timespacing) {
            delay(5);
        }
    }
}

void charTesting(unsigned char letter) {
    char chars[4];
    chars[0] = letter;
    for (uint8_t i = 1; i < 4; i++) {
        char temp = letter - i;
        if (temp < 0x20) {
            char offset = 0x20 - temp;
            temp = 0x7F - offset;
        }
        chars[i] = temp;
    }

    Serial.println(chars);
    display.updateString(chars, 4);
}
