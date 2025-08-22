#ifndef ENCODER_H
#define ENCODER_H

#include <esp_attr.h>
#include <esp32-hal-timer.h>

class Encoder {
public:
  Encoder();
  ~Encoder();

  static void IRAM_ATTR ENA_IRQ(void* encoder) {
    ((Encoder*)encoder)->updateA();
  }
  static void IRAM_ATTR ENB_IRQ(void* encoder) {
    ((Encoder*)encoder)->updateB();
  }

  void attach(unsigned int apin, unsigned int bpin);
  void updateA();
  void updateB();

  long long getCount();
  void setCount(long long count);
  void clear();

  // 获取自上次调用以来的增量
  long getDeltaAndClear();

private:
  volatile long long encoderCount = 0;
  unsigned int A_pin;
  unsigned int B_pin;
};

#endif
