#ifndef PushButton_h
#define PushButton_h

#include "Arduino.h"
class PushButton
{
  public:
    PushButton(int pin);
    byte getState();
    byte getLastState();
    void readPin();
  private:
    int _pin;  
    byte _state = LOW; 
    byte _lastState = LOW; 
  
};

#endif