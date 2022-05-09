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
  for (int i = 0; i < 5; i++) {
    this->column[i] = columns[i];
  }
  this->data = data;
  this->clock = clock;
  this->num = num;
  
  // Pull everything low at start to prevent floating pins and latch the shift registers 
  for (int i = 0; i < 5; i++) {
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
    for (int k = 0; k < 5; k++) { // k is column number
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
    digitalWrite(this->clock, HIGH); // Start clock
    digitalWrite(this->data, (out >> pos) & 1); // write specific pixel
    delayMicroseconds(1); // wait to latch
    digitalWrite(this->clock, LOW); // end clock pulse
    delayMicroseconds(1);
  }
}

void HDSP_200X::updateString(char *newString) {
  len = strlen(newString);
  this->updateString(newString, len);
}

void HDSP_200X::updateString(char *newString, unsigned int len) {
  noInterrupts();
  free(this->currentString);
  this->len = len;
  this->currentString = malloc(len + 1);
  for (int i = 0; i < len + 1; i++) {
    currentString[i] = NULL;  //refill
  }
  for (int i = 0; i < len; i++) {
    currentString[i] = newString[i];  //copy
  }
  interrupts();
}

void HDSP_200X::clear(void) {
  for (int i = 0; i < this->num; i++) {
    this->writeData(0); // send all zeros (blank spaces)
  }
}

void HDSP_200X::pause(void) {
  ITimer1.pauseTimer();
}

void HDSP_200X::draw(void) {
  ITimer1.resumeTimer();
}

unsigned char *HDSP_200X::getCurrentString() {
  return this->currentString;
}

static void HDSP_200X::ISRHandle(void) {
  thisDisplay->displayUpdate();
}

void HDSP_200X::displayUpdate(void) {
  unsigned char chars[(4 * num) + 1][5] = {0}; // assumed 10 max for number of displays, for allocation perposes
  for (int i = 0; i < 4 * this->len; i++) { // shift down to match actual matrix map
    for (int j = 0; j < 5; j++) {
      chars[i][j] = pgm_read_byte_near((unsigned char *)&char_data[(this->currentString[i] - 0x20)][j]); // read from map
    }
  }

  for (int i = 0; i < 5; i++) {
    for (int j = (num - 1); j >= 0; j--) { // write last 4 first, then write the displays used
      // see above for shifting information
      unsigned long out = (((unsigned long) chars[(4 * j) + 0][i]) << 21) + (((unsigned long) chars[(4 * j) + 1][i]) << 14) + (((unsigned long) chars[(4 * j) + 2][i]) << 7) + ((unsigned long) chars[(4 * j) + 3][i]);
      this->writeData(out);
    }
    digitalWrite(this->column[i], HIGH); 
    delayMicroseconds(1200); 
    digitalWrite(this->column[i], LOW);
  }
}
