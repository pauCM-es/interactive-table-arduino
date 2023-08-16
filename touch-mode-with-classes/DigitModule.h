#ifndef DigitModule_h
#define DigitModule_h

#include "Arduino.h"

class DigitModule {
public:
  DigitModule(int segmentsPins[7]);
  void show(int number);
  void clean();
private:
  unsigned int _segmentsPins[7];
  boolean _digitList[10][7] = {
    { 1, 1, 1, 1, 1, 1, 0 },  // 0
    { 0, 1, 1, 0, 0, 0, 0 },  // 1
    { 1, 1, 0, 1, 1, 0, 1 },  // 2
    { 1, 1, 1, 1, 0, 0, 1 },  // 3
    { 0, 1, 1, 0, 0, 1, 1 },  // 4
    { 1, 0, 1, 1, 0, 1, 1 },  // 5
    { 1, 0, 1, 1, 1, 1, 1 },  // 6
    { 1, 1, 1, 0, 0, 0, 0 },  // 7
    { 1, 1, 1, 1, 1, 1, 1 },  // 8
    { 1, 1, 1, 0, 0, 1, 1 },  // 9
  };
};

DigitModule::DigitModule(int segmentsPins[7]) {
  for (int i = 0; i < 7; i++) {
    pinMode(segmentsPins[i], OUTPUT);
    _segmentsPins[i] = segmentsPins[i];
  }
}

void DigitModule::show(int number) {
  for (int i = 0; i < 7; i++) {
    int pin = _segmentsPins[i];
    boolean state = digitalRead(pin);
    boolean newState = _digitList[number][i];
    if (state != newState) {
      digitalWrite(pin, newState);
    }
  }
}

void DigitModule::clean() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(_segmentsPins[i], LOW);
  }
}

#endif