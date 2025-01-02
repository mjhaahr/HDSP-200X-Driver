/***** HDSP-200x Driver ***** 

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/


#include "HDSP_200X.h"

//static HDSP_200X *thisDisplay; //singeton object for the ISR to access becuase it is static


char HDSP_200X::chars[(4 * MAX_DISPLAYS) + 1][5] = {0};


HDSP_200X::HDSP_200X(char* columns, char data, char clock, unsigned char num) {
  for (int i = 0; i < NUM_COLS; i++) {
    this->column[i] = columns[i];
  }
  this->data = data;
  this->clock = clock;
  this->num = num;
  
  // Pull everything low at start to prevent floating pins and latch the shift registers 
  for (int i = 0; i < NUM_COLS; i++) {
    pinMode(this->column[i], OUTPUT);
    digitalWrite(this->column[i], LOW);
  }
  pinMode(this->data, OUTPUT);
  pinMode(this->clock, OUTPUT);
  digitalWrite(this->data, LOW);
  digitalWrite(this->clock, LOW);
  
  //thisDisplay = this;
}

// TODO: test without latch time
void HDSP_200X::testDisplay(char num) {
  for (int i = 0; i < (7 * 4 * num); i++) {
    digitalWrite(clock, HIGH); // Latch clock
    digitalWrite(data, LOW); // shift out a 0 clear display
    digitalWrite(clock, LOW); // end clock pulse
  }
  
  // First one needs to be handled seperately
  digitalWrite(clock, HIGH); // Latch clock
  digitalWrite(data, HIGH); // shift out a 1 (start it off with a one)
  digitalWrite(clock, LOW); // end clock pulse
  
  for (int i = 0; i < (7 * 4 * num); i++) { // loops until clear
    for (int k = 0; k < NUM_COLS; k++) { // k is column number
      // directly addressing columns for void character control
      digitalWrite(column[k], HIGH); 
      delayMicroseconds(2000); 
      digitalWrite(column[k], LOW); 
      delayMicroseconds(1000); 
    }
    digitalWrite(clock, HIGH); // Latch clock
    digitalWrite(data, LOW); // shift out a 0
    digitalWrite(clock, LOW); // end clock pulse
  }
}
    
void HDSP_200X::writeData(unsigned long out) {
  out = out & 0x0FFFFFFF;  // mask to only 28 bits, shift register size
  for (char pos = 0; pos < 28; pos++) { // bitbang data
    digitalWrite(clock, HIGH); // Start clock
    digitalWrite(data, (out >> pos) & 1); // write specific pixel
    digitalWrite(clock, LOW); // end clock pulse
  }
}

void HDSP_200X::updateString(char *newString) {
  len = strlen(newString);
  this->updateString(newString, len);
}

void HDSP_200X::updateString(char *newString, unsigned int len) {
  free(currentString);
  this->len = len;
  currentString = (char *) malloc(len + 1);
  for (unsigned int i = 0; i < len + 1; i++) {
    currentString[i] = 0;  //refill
  }
  for (unsigned int i = 0; i < len; i++) {
    currentString[i] = newString[i];  //copy
  }
}

void HDSP_200X::clear(void) {
  for (int i = 0; i < this->num; i++) {
    writeData(0); // send all zeros (blank spaces)
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
  //thisDisplay->displayUpdate();
}

void HDSP_200X::displayUpdate(void) {
  for (int i = 0; i < 4 * len; i++) { // shift down (by ' ', 0x20) to match actual matrix map from chars (ignore control chars)
    for (int j = 0; j < 5; j++) {
      chars[i][j] = pgm_read_byte_near((char *)&char_data[(currentString[i] - ' ')][j]); 
      // read from map, subtract ' ' to get to visible characters
      // read from program memory
    }
  }
  for (int i = 0; i < 5; i++) {
    for (int j = (num - 1); j >= 0; j--) { // write last 4 first, then write the displays used
      // see above for shifting information
      unsigned long out = 
        (((unsigned long) chars[(4 * j) + 0][i]) << (3 * NUM_ROWS)) +
        (((unsigned long) chars[(4 * j) + 1][i]) << (2 * NUM_ROWS)) +
        (((unsigned long) chars[(4 * j) + 2][i]) << NUM_ROWS) +
        ((unsigned long) chars[(4 * j) + 3][i]);
      writeData(out);
    }
    digitalWrite(column[i], HIGH); 
    delay(2); 
    digitalWrite(column[i], LOW);
  }
}
