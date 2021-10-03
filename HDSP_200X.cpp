/***** HDSP-200x Driver ***** 

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/


#include "HDSP_200X.h"


HDSP_200X::HDSP_200X(char* columns, char data, char clock) {
  for (int i = 0; i < 5; i++) {
    this->column[i] = columns[i];
  }
  this->data = data;
  this->clock = clock;
}

void HDSP_200X:init() {
  // Pull everything low at start to prevent floating pins and latch the shift registers 
  for (int i = 0; i < 5; i++) {
    pinMode(this->column[i], OUTPUT);
    digitalWrite(this->column[i], LOW);
  }
  pinMode(this->data, OUTPUT);
  pinMode(this->clock, OUTPUT);
  digitalWrite(this->data, LOW);
  digitalWrite(this->clock, LOW);
}


void HDSP_200X::writeData(unsigned long out) {
  out = out & 0x0FFFFFFF;  // mask to only 28 bits, shift register size
  for (char pos = 0; pos < 28; pos++) { // bitbang data
    digitalWrite(this->clock, HIGH); // Start clock
    digitalWrite(this->data, (out >> pos) & 1); // write specific pixel
    delayMicroseconds(2); // wait to latch
    digitalWrite(this->clock, LOW); // end clock pulse
    delayMicroseconds(2);
  }
}

void HDSP_200X::clear(char num) {
  for (int i = 0; i < num; i++) {
    this->writeData(0); // send all zeros (blank spaces)
  }
}

void HDSP_200X::writeFourChars(unsigned char *text) {
  this->currentString = text; // update current string
  unsigned char chars[4][5]; // character mapping
  for (int i = 0; i < 4; i++) { // shift down to match actual matrix map
    for (int j = 0; j < 5; j++) {
	    chars[i][j] = pgm_read_byte_near((unsigned char *)&char_data[(text[i] - 0x20)][j]); // read from map
    }
  }

  for (int i = 0; i < 5; i++) {
    // shift mapped data around to get the data in the correct orientation (for each column: char1, char2, char3, char4, each char is 7 nits)
    unsigned long out = (((unsigned long) chars[0][i]) << 21) + (((unsigned long) chars[1][i]) << 14) + (((unsigned long) chars[2][i]) << 7) + ((unsigned long) chars[3][i]);
    this->writeData(out); // write the data
    digitalWrite(this->column[i], HIGH); // enable that column
    delayMicroseconds(2500); // relying on POV
    digitalWrite(this->column[i], LOW); // turn off column
  }
}

void HDSP_200X::writeNChars(char num, unsigned char *text) { // writes to num * 4 characters, similar to above, just more
  this->currentString = text;
  unsigned char chars[4 * 10][5]; // assumed 10 max for number of displays, for allocation perposes
  for (int i = 0; i < 4 * num; i++) { // shift down to match actual matrix map
    for (int j = 0; j < 5; j++) {
      chars[i][j] = pgm_read_byte_near((unsigned char *)&char_data[(text[i] - 0x20)][j]); // read from map
    }
  }

  for (int i = 0; i < 5; i++) {
    for (int j = (num - 1); j >= 0; j--) { // write last 4 first, then write the displays used
      // see above for shifting information
      unsigned long out = (((unsigned long) chars[(4 * j) + 0][i]) << 21) + (((unsigned long) chars[(4 * j) + 1][i]) << 14) + (((unsigned long) chars[(4 * j) + 2][i]) << 7) + ((unsigned long) chars[(4 * j) + 3][i]);
      this->writeData(out);
    }
    digitalWrite(this->column[i], HIGH); 
    delayMicroseconds(2500); 
    digitalWrite(this->column[i], LOW);
  }
}


unsigned char *HDSP_200X::getCurrentString() {
  return this->currentString;
}
