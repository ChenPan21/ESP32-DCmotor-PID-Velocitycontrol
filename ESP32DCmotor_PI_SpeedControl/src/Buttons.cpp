#include "Buttons.h"

void Buttons::begin(uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, bool activeHigh) {
  b1.pin = k1; b2.pin = k2; b3.pin = k3; b4.pin = k4;
  b1.activeHigh = b2.activeHigh = b3.activeHigh = b4.activeHigh = activeHigh;
  pinMode(k1, INPUT);
  pinMode(k2, INPUT);
  pinMode(k3, INPUT);
  pinMode(k4, INPUT);
}

void Buttons::updateOne(ButtonState& b) {
  bool level = (digitalRead(b.pin) == (b.activeHigh ? HIGH : LOW));
  unsigned long now = millis();

  b.evShort = false; b.evLongRepeat = false;

  if (level != b.levelPrev) {
    b.levelPrev = level;
    b.tChange = now;
    if (level) { b.tPress = now; b.longMode = false; }
    else {
      unsigned long held = now - b.tPress;
      if (!b.longMode && held >= debounceMs && held < longPressMs) b.evShort = true;
      b.longMode = false;
    }
  } else {
    if (level) {
      unsigned long held = now - b.tPress;
      if (!b.longMode && held >= longPressMs) {
        b.longMode = true; b.tRepeat = now; b.evLongRepeat = true;
      } else if (b.longMode && (now - b.tRepeat >= repeatMs)) {
        b.tRepeat = now; b.evLongRepeat = true;
      }
    }
  }
}

void Buttons::update() {
  updateOne(b1); updateOne(b2); updateOne(b3); updateOne(b4);
}
