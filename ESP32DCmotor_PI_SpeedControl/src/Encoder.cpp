#include "Encoder.h"
#include <Arduino.h>

Encoder::Encoder() {}
Encoder::~Encoder() {}

void Encoder::attach(unsigned int apin, unsigned int bpin) {
  A_pin = apin;
  B_pin = bpin;
  pinMode(A_pin, INPUT);
  pinMode(B_pin, INPUT);
  attachInterruptArg(A_pin, ENA_IRQ, this, CHANGE);
  attachInterruptArg(B_pin, ENB_IRQ, this, CHANGE);
}

void Encoder::updateA() {
  if (digitalRead(A_pin) ^ digitalRead(B_pin)) {
    encoderCount--;
  } else {
    encoderCount++;
  }
}

void Encoder::updateB() {
  if (digitalRead(A_pin) ^ digitalRead(B_pin)) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

long long Encoder::getCount() {
  return encoderCount;
}

void Encoder::setCount(long long count) {
  encoderCount = count;
}

void Encoder::clear() {
  encoderCount = 0;
}

long Encoder::getDeltaAndClear() {
  static long long lastCount = 0;
  long long currentCount = getCount();
  long delta = currentCount - lastCount;
  lastCount = currentCount;
  return delta;
}
