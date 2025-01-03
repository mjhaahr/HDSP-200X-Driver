/***** HDSP-200x Driver *****

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/

// TODO: DOCUMENTATION (and keywords)
// TODO: setup timer interrupt
// TODO: figure out actual timing requirements (rate and so on)
// TODO: ISR logic:
//      Turn off active column
//      Write new column into buffer
//      Turn on new column
// Maybe TODO: Add brightness control (PWM the column data? or just disable after a time)
//      Two timer cycles, one starts the display, the other turns off the column after a bit

// If changing the string and actively displaying, pause and unpause behind the scenes

#include "HDSP_200X.h"

//static HDSP_200X *thisDisplay; //singeton object for the ISR to access becuase it is static

HDSP_200X::HDSP_200X(uint8_t* columns, uint8_t data, uint8_t clock, uint8_t num) {
    // Setup Column Control Lines and pull low to prevent floating pins
    for (uint8_t col = 0; col < NUM_COLS; col++) {
        this->column[col] = columns[col];

        pinMode(this->column[col], OUTPUT);
        digitalWrite(this->column[col], LOW);
    }

    // Setup the clock and data lines
    this->clock = clock;
    pinMode(this->clock, OUTPUT);
    digitalWrite(this->clock, LOW);

    this->data = data;
    pinMode(this->data, OUTPUT);
    digitalWrite(this->data, LOW);

    this->num = num;

    // Passing in empty string
    updateString((char *) "");

    //thisDisplay = this;
}

// TODO: test without latch time
void HDSP_200X::testDisplay(uint8_t num) {
    // Clear the display
    clear();

    // First one needs to be handled seperately
    digitalWrite(clock, HIGH); // Latch clock
    digitalWrite(data, HIGH); // shift out a 1 (start it off with a one)
    digitalWrite(clock, LOW); // end clock pulse

    // Shift the single bit around until it leaves the display
    for (int i = 0; i < (7 * 4 * num); i++) { // loops until clear
        for (int col = 0; col < NUM_COLS; col++) {
            // directly addressing columns for void character control
            digitalWrite(column[col], HIGH);
            delayMicroseconds(2000);
            digitalWrite(column[col], LOW);
            delayMicroseconds(1000);
        }
        digitalWrite(clock, HIGH); // Latch clock
        digitalWrite(data, LOW); // shift out a 0
        digitalWrite(clock, LOW); // end clock pulse
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

    // Raw Column Data to copy into
    uint8_t columnData[NUM_COLS][4 * MAX_DISPLAYS];

    // Copy string and fill column data storage
    for (uint8_t i = 0; i < len; i++) {
        currentString[i] = newString[i];  //copy string

        // Fill the column buffer
        uint8_t charOffset = (currentString[i] - ' ');
        for (uint8_t col = 0; col < NUM_COLS; col++) {
            columnData[col][i] = pgm_read_byte_near(&char_data[charOffset][col]);
        }
    }

    // For the unfilled spaces, just add zeroes
    for (uint8_t i = (len + 1); i < (4 * this->num); i++) {
        for (uint8_t col = 0; col < NUM_COLS; col++) {
            columnData[col][i] = 0;
        }
    }

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
}

void HDSP_200X::clear(void) {
    for (uint8_t i = 0; i < this->num; i++) {
        writeColumn(0); // send all zeros (blank spaces)
    }
}

void HDSP_200X::pause(void) {
    //ITimer1.pauseTimer();
}

void HDSP_200X::draw(void) {
    //ITimer1.resumeTimer();
}

char *HDSP_200X::getCurrentString() {
    return currentString;
}

void HDSP_200X::ISRHandle(void) {
    //thisDisplay->writeBuffer();
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
