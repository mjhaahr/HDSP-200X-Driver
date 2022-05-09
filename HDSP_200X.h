/***** HDSP-200x Driver ***** 

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "matrix.h"

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
     * @param num =1 The number of 4 character display the dot should travel along, default is one display
     */
    void testDisplay(char num = 1);

    /**
     * Gets the current text
     * @return The current text displayed on the display as a char*
     */
    unsigned char *getCurrentString(void);
  
  private:
    char column[5];  // The column pins
    char data;  // The data pin
    char clock;  // The clock pin
    unsigned char num; // The number of display units
    unsigned char *currentString;  // The current string being displayed
    unsigned char len;  // The length of currentString

    /**
     * Writes a 28 bit stream to the displays shift registers
     * @param the data to shift out
     */
    void writeData(unsigned long out);
    
    /**
     * Timer ISR for display updaing
     */
     static void ISRHandle(void);

     /**
      * The actual ISR handler, non-static method
      */
     void displayUpdate(void);

};

#endif
