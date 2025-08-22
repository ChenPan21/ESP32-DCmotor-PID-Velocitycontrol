#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

struct ButtonState {
  uint8_t pin;
  bool activeHigh;             // true: 高电平按下
  bool levelPrev = false;
  unsigned long tChange = 0;
  unsigned long tPress = 0;
  bool longMode = false;
  unsigned long tRepeat = 0;

  bool evShort = false;
  bool evLongRepeat = false;
};

class Buttons {
public:
  void begin(uint8_t k1, uint8_t k2, uint8_t k3, uint8_t k4, bool activeHigh = true);
  void update();
  ButtonState& left()  { return b1; }   // 翻页/左
  ButtonState& right() { return b2; }   // 翻页/右
  ButtonState& inc()   { return b3; }   // 参数+/目标速度+
  ButtonState& dec()   { return b4; }   // 参数-/目标速度-

private:
  void updateOne(ButtonState& b);

  ButtonState b1{}, b2{}, b3{}, b4{};
  const unsigned long debounceMs = 30;
  const unsigned long longPressMs = 350;
  const unsigned long repeatMs = 150;
};

#endif
