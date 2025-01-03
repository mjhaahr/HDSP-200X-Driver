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
        HDSP_200X(uint8_t *columns, uint8_t data, uint8_t clock, uint8_t num);

        /**
         * Scrolls a dot down each pixel in the number of displays used
         */
        void testDisplay(void);

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
        void updateString(char *newString,uint8_t len);

        /**
         * Gets the current text
         * @return The current text displayed on the display as a char*
         */
        char *getCurrentString(void);

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
         * The Timer ISR
         */
        static void ISRHandle(void);

        /**
         * Draws a Single entry of the column Buffer
         */
        void drawSingleBuffer(void);

        /**
         * Flushes the current display buffer to the displays shift
         */
        void writeBuffer(void);

    private:
        uint8_t column[5];      // The column pins
        uint8_t data;           // The data pin
        uint8_t clock;          // The clock pin
        uint8_t num;   // The number of display units
        char *currentString; // The current string being displayed
        uint8_t len;   // The length of currentString

        volatile uint8_t activeCol;

        // Compressed Column Data
        volatile uint32_t columnBuffer[NUM_COLS][MAX_DISPLAYS];

        /**
         * Writes a 28 bit stream for a single display module's shift registers
         * @param the data to shift out
         */
        void writeColumn(uint32_t colData);
};

#endif  /* HDSP_200X_H */
