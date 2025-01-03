/***** HDSP-200x Driver *****

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
Uses TimerOne.h for timer interrupt generation
*/

// TODO: DOCUMENTATION (and keywords)
// Maybe TODO: Add brightness control (add a Brightness line, which gets PWM'ed (that's what VB is for))

// TODO: debate between only allowing one and allow multiple "Instances"

#include "HDSP_200X.h"
#include <TimerOne.h>

static HDSP_200X *thisDisplay; //singeton object for the ISR to access becuase it is static

HDSP_200X::HDSP_200X(uint8_t* columns, uint8_t data, uint8_t clock, uint8_t num) {

    // Setup the clock and data lines and pull low to prevent floating pins
    this->clock = clock;
    pinMode(this->clock, OUTPUT);
    digitalWrite(this->clock, LOW);

    this->data = data;
    pinMode(this->data, OUTPUT);
    digitalWrite(this->data, LOW);

    // Setup Column Control Lines and pull low to prevent floating pins
    for (uint8_t col = 0; col < NUM_COLS; col++) {
        this->column[col] = columns[col];

        pinMode(this->column[col], OUTPUT);
        digitalWrite(this->column[col], LOW);
    }

    this->num = num;

    // Passing in empty string
    updateString((char *) "");

    thisDisplay = this;

    Serial.begin(9600);
    Serial.println("Test");

    testDisplay();

    Serial.println("After Test");

    // Draw a column on the display every 2ms
    Timer1.initialize(2000);
    pause();
    Timer1.attachInterrupt(HDSP_200X::ISRHandle);
}

void HDSP_200X::testDisplay(void) {
    // Clear the display
    clear();
    Serial.println("After Clear");

    // First one needs to be handled seperately
    digitalWrite(clock, HIGH); // Latch clock
    digitalWrite(data, HIGH); // shift out a 1 (start it off with a one)
    digitalWrite(clock, LOW); // end clock pulse
    Serial.println("After Writes");

    // Shift the single bit around until it leaves the display
    for (int i = 0; i < (7 * 4 * num); i++) { // loops until clear
        Serial.println("Start");
        for (int col = 0; col < NUM_COLS; col++) {
            // directly addressing columns for void character control
            digitalWrite(column[col], HIGH);
            delay(3);
            digitalWrite(column[col], LOW);
            delay(2);
        }
        digitalWrite(clock, HIGH); // Latch clock
        digitalWrite(data, LOW); // shift out a 0
        digitalWrite(clock, LOW); // end clock pulse
        // Does not reach here
        Serial.print("Dot: ");
        Serial.println(i);
        delay(10);
    }
}

void HDSP_200X::writeColumn(uint32_t colData) {
    colData = colData & 0x0FFFFFFF;  // mask to only 28 bits, single char size
    for (uint8_t pos = 0; pos < 28; pos++) { // bitbang data
        digitalWrite(clock, HIGH); // Start clock
        digitalWrite(data, (colData >> pos) & 1); // write specific pixel
        digitalWrite(clock, LOW); // end clock pulse
    }
}

void HDSP_200X::updateString(char *newString) {
    len = strlen(newString);
    this->updateString(newString, len);
}

void HDSP_200X::updateString(char *newString, uint8_t len) {
    free(currentString);
    this->len = len;
    currentString = (char *) malloc(len + 1);
    // Add terminator to string
    currentString[len] = 0;

    // Raw Column Data to copy into (default is spaces)
    uint8_t columnData[NUM_COLS][4 * MAX_DISPLAYS] = {' '};

    // TODO: only copy in legit characters (within range), and update length if too short

    // Copy string and fill column data storage
    for (uint8_t i = 0; i < len; i++) {
        char newChar = newString[i];
        // If out of bounds, update the length and stop storing (everything should be filled with spaces)
        if ((newChar < 0x20) || (newChar >= 0x7F)) {
            this->len = i;
            break;
        }
        currentString[i] = newChar;  //copy string

        // Fill the column buffer
        uint8_t charOffset = (currentString[i] - ' ');
        for (uint8_t col = 0; col < NUM_COLS; col++) {
            columnData[col][i] = pgm_read_byte_near(&char_data[charOffset][col]);
        }
    }

    // Prevent interrupts while updating the display
    noInterrupts();
    // Compress the column data into the column buffers
    // Loop over all columns
    for (uint8_t col = 0; col < NUM_COLS; col++) {
        // Store the buffer for each displays, starting with the last display (as first entry into the buffer)
        for (uint8_t i = 0; i < num; i++) {
            // Shift mapped data around to get the data in the correct orientation
            // (for each column: char0, char1, char2, char3: each char is 7 bits)
            uint8_t charOffset = 4 * ((num - 1) - i);
            columnBuffer[col][i] =
                (((uint32_t) columnData[col][charOffset + 0]) << (3 * NUM_ROWS)) |
                (((uint32_t) columnData[col][charOffset + 1]) << (2 * NUM_ROWS)) |
                (((uint32_t) columnData[col][charOffset + 2]) << (1 * NUM_ROWS)) |
                (((uint32_t) columnData[col][charOffset + 3]) << (0 * NUM_ROWS));
        }
    }
    interrupts();
}

void HDSP_200X::clear(void) {
    for (uint8_t i = 0; i < this->num; i++) {
        writeColumn(0); // send all zeros (blank spaces)
    }
}

void HDSP_200X::pause(void) {
    this->activeCol = 0;
    Timer1.stop();
}

void HDSP_200X::draw(void) {
    this->activeCol = 0;
    Timer1.start();
}

char *HDSP_200X::getCurrentString() {
    return currentString;
}

/**
 * The Timer ISR
 */
void HDSP_200X::ISRHandle(void) {
    thisDisplay->drawSingleBuffer();
}

/**
 * Draws a Single entry of the column Buffer
 */
void HDSP_200X::drawSingleBuffer(void) {
    // ISR logic:
    //      Turn off active column
    //      Write new column into buffer
    //      Turn on new column

    digitalWrite(column[activeCol], LOW);
    activeCol = (activeCol + 1) % NUM_COLS;
    for (uint8_t i = 0; i < this->num; i++) {
        writeColumn(columnBuffer[activeCol][i]);
    }
    digitalWrite(column[activeCol], HIGH);
}

void HDSP_200X::writeBuffer(void) {
    for (uint8_t col = 0; col < NUM_COLS; col++) {
        // Write the data from the column buffer to the display
        for (uint8_t i = 0; i < this->num; i++) {
            writeColumn(columnBuffer[col][i]);
        }
        digitalWrite(column[col], HIGH);
        delay(2);
        digitalWrite(column[col], LOW);
    }
}
