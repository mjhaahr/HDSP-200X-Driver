/***** HDSP-200x Driver ***** 

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "matrix.h"

//#ifndef HDSP_200X
//#define HDSP_200X

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
     */
    HDSP_200X(char *columns, char data, char clock);
    
    /**
     * Clears the number of displays provided (default is one), rewrites all data to zero
     * @param num = 1 The number of 4 character displays to clear, default is one display
     */
    void clear(char num = 1);
    
    /**
     * Scrolls a dot down each pixel in the number of displays used
     * @param num =1 The number of 4 character display the dot should travel along, default is one display
     */
    void testDisplay(char num = 1);
    
    /**
    * Writes four characters to the display
    * @param text The text stream to write
    */
    void writeFourChars(unsigned char *text);
    
    /**
     * Writes n*4 characters to the display, used for display chaining
     * @param num The number of displays to write to
     * @param text The text stream to write to the displays
     */
    void writeNChars(char num, unsigned char *text);

    /**
     * Gets the current text
     * @return The current text displayed on the display as a char*
     */
    unsigned char *getCurrentString(void);
  
  private:
    char column[5];  /// The column pins
    char data;  /// The data pin
    char clock;  /// The clock pin
    unsigned char *currentString;  /// The current string being displayed

    /**
     * Writes a 28 bit stream to the displays shift registers
     * @param the data to shift out
     */
    void writeData(unsigned long out);

};

//#endif
