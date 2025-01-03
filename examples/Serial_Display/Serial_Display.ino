/***** HDSP-200x Driver Example*****
Serial Display
 - Displays the passed in string from the Serial buffer
 - If a control character is recieved, will clear the string next time around
 - If CTRL-C is recieved, clears now

Written By Matthew Haahr

Utilizes HDSP-200X Display driver class
*/

#include <HDSP_200X.h>
#include <matrix.h>

#define NUM     1
#define LEN     NUM * 4

uint8_t column[] = {2, 3, 4, 5, 6};
uint8_t data = 7;
uint8_t clock = 8;

HDSP_200X display = HDSP_200X(column, data, clock, NUM);

char str[LEN] = {0};
bool toClear = false;

void setup() {
    Serial.begin(9600);
    display.draw();
}

void loop() {
    if (Serial.available()) {
        char newChar = Serial.read();

        if (newChar == 0x03) {
            // If new character is CTRL-C, clear immediately
            clearString();
            display.updateString(str, LEN);

        } else if ((newChar >= 0x20) && (newChar < 0x7F)) {
            // If character is within displayable range
            if (toClear) {
                clearString();
                toClear = false;
            }

            // Shift all characters over
            for (uint8_t i = 0; i < (LEN - 1); i++) {
                char temp = str[i];
                // Exit if reading terminator characters
                if (temp == 0) {
                    break;
                } else {
                    str[i + 1] = temp;
                }
            }
            // Load in new character
            str[0] = newChar;
            // Update the string
            display.updateString(str, LEN);

        } else {
            // Else clear on next valid character
            toClear = true;
        }
    }
    delay(5);
}

void clearString(void) {
    for (uint8_t i = 0; i < LEN; i++) {
        str[i] = 0;
    }
}
