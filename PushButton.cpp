#include "Arduino.h"
#include "PushButton.h"

PushButton::PushButton(int pin)
{ 
  _pin = pin;
  pinMode(pin, INPUT_PULLUP);
}

void PushButton::readPin(){
    _lastState = _state;
    _state = digitalRead(_pin);
}


byte PushButton::getState(){
  return _state;
}


byte PushButton::getLastState(){
  return _lastState;
}


