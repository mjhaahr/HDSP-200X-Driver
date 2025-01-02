/***** HDSP-200x Driver *****

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/

#include "matrix.h"
#include <Arduino.h>
#include <avr/pgmspace.h>

#ifndef HDSP_200X_H
#define HDSP_200X_H

#define NUM_COLS 5
#define NUM_ROWS 7

#define MAX_DISPLAYS 8

/**
* HDSP Display Class, contains all methods and data
*/
class HDSP_200X {
    public:
        /**
         * Constructor Method
         * @param columns The column control pins, in order
         * @param data The data pin
         * @param clock The clock pin
         * @param num The number of 4 character display units
         */
        HDSP_200X(char *columns, char data, char clock, unsigned char num);

        /**
         * Updates the string stored in the memory (frees and reallocates)
         * @param newString Pointer ton the new string
         */
        void updateString(char *newString);

        /**
         * Updates the string stored in mem
         * @Overload
         * @param newString newString pointer
         * @param len The length of the string
         */
        void updateString(char *newString, unsigned int len);

        /**
         * Clears the displays, rewrites all data to zero
         */
        void clear(void);

        /**
         * Pauses the display (stops the update calls)
         */
        void pause(void);

        /**
         * Restarts the display (continues the update calls)
         */
        void draw(void);

        /**
         * Scrolls a dot down each pixel in the number of displays used
         * @param num =1 The number of 4 character display the dot should travel
         * along, default is one display
         */
        void testDisplay(char num = 1);

        /**
         * Gets the current text
         * @return The current text displayed on the display as a char*
         */
        char *getCurrentString(void);

    private:
        uint8_t column[5];      // The column pins
        uint8_t data;           // The data pin
        uint8_t clock;          // The clock pin
        uint8_t num;   // The number of display units
        char *currentString; // The current string being displayed
        uint8_t len;   // The length of currentString

        // Compressed Column Data
        static uint32_t columnBuffer[NUM_COLS][MAX_DISPLAYS];

        /**
         * Writes a 28 bit stream for a single display module's shift registers
         * @param the data to shift out
         */
        void writeColumn(uint32_t colData);

        /**
         * Timer ISR for display updaing
         */
        static void ISRHandle(void);

        /**
         * Flushes the current display buffer to the displays shift
         */
        void writeBuffer(void);
};

#endif
