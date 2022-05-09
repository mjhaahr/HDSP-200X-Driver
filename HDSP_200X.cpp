/***** HDSP-200x Driver ***** 

Written By Matthew Haahr

Utilizes pgmspace.h and matrix.h for font mapping
*/


#include "HDSP_200X.h"

#define USE_TIMER_1     true
#define USE_TIMER_2     false
#define USE_TIMER_3     false
#define USE_TIMER_4     false
#define USE_TIMER_5     false
#include <TimerInterrupt.h>

static HDSP_200X *thisDisplay; //singeton object for the ISR to access becuase it is static


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
  
  ITimer1.init();
  ITimer1.attachInterrupt(60, ISRHandle);
  ITimer1.pauseTimer();
  thisDisplay = this;
}

// TODO: test without latch time
void HDSP_200X::testDisplay(char num) {
  // First one needs to be handled seperately
  digitalWrite(clock, HIGH); // Latch clock
  digitalWrite(data, HIGH); // shift out a 1 (start it off with a one
  delayMicroseconds(1); // wait to latch
  digitalWrite(clock, LOW); // end clock pulse
  delayMicroseconds(1);
  
  for (int i = 0; i < (7 * 4 * num); i++) { // loops until clear
    for (int k = 0; k < NUM_COLS; k++) { // k is column number
      // directly addressing columns for void character control
      digitalWrite(column[k], HIGH); 
      delayMicroseconds(1000); 
      digitalWrite(column[k], LOW); 
      delayMicroseconds(750); 
    }
    digitalWrite(clock, HIGH); // Latch clock
    digitalWrite(data, LOW); // shift out a 0
    delayMicroseconds(1); // wait to latch
    digitalWrite(clock, LOW); // end clock pulse
    delayMicroseconds(1);
  }
}
    
// TODO: test without latch time
void HDSP_200X::writeData(unsigned long out) {
  out = out & 0x0FFFFFFF;  // mask to only 28 bits, shift register size
  for (char pos = 0; pos < 28; pos++) { // bitbang data
    digitalWrite(clock, HIGH); // Start clock
    digitalWrite(data, (out >> pos) & 1); // write specific pixel
    delayMicroseconds(1); // wait to latch
    digitalWrite(clock, LOW); // end clock pulse
    delayMicroseconds(1);
  }
}

void HDSP_200X::updateString(char *newString) {
  len = strlen(newString);
  this->updateString(newString, len);
}

void HDSP_200X::updateString(char *newString, unsigned int len) {
  noInterrupts();
  free(currentString);
  this->len = len;
  currentString = (unsigned char *) malloc(len + 1);
  for (int i = 0; i < len + 1; i++) {
    currentString[i] = 0;  //refill
  }
  for (int i = 0; i < len; i++) {
    currentString[i] = newString[i];  //copy
  }
  interrupts();
}

void HDSP_200X::clear(void) {
  for (int i = 0; i < this->num; i++) {
    writeData(0); // send all zeros (blank spaces)
  }
}

void HDSP_200X::pause(void) {
  ITimer1.pauseTimer();
}

void HDSP_200X::draw(void) {
  ITimer1.resumeTimer();
}

unsigned char *HDSP_200X::getCurrentString() {
  return currentString;
}

void HDSP_200X::ISRHandle(void) {
  thisDisplay->displayUpdate();
}

void HDSP_200X::displayUpdate(void) {
  unsigned char chars[(4 * MAX_DISPLAYS) + 1][5] = {0};  // allocate to max number of characters
  for (int i = 0; i < 4 * len; i++) { // shift down (by ' ') to match actual matrix map from chars (ignore control chars)
    for (int j = 0; j < 5; j++) {
      chars[i][j] = pgm_read_byte_near((unsigned char *)&char_data[(currentString[i] - ' ')][j]); 
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
    delayMicroseconds(1200); 
    digitalWrite(column[i], LOW);
  }
}
