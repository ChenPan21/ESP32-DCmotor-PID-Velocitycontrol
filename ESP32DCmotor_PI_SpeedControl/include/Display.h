#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

enum UiPage : uint8_t { PAGE_STATUS = 0, PAGE_ENCODER = 1, PAGE_PID = 2 };

class DisplayUI {
public:
  bool begin(uint8_t addr = 0x3C, int16_t w = 128, int16_t h = 64);
  void setPage(UiPage p);
  UiPage page() const { return current; }

  // 数据接口
  void setStatus(float tgtSpeed, float lSpd, float rSpd, int lPWM, int rPWM);
  void setPid(float kp, float ki, float kd);

  void render();     // 刷一帧

private:
  void drawStatus();
  void drawEncoder();
  void drawPid();

  Adafruit_SSD1306 d{128, 64, &Wire, -1};
  UiPage current = PAGE_STATUS;

  // 状态缓存
  float targetSpeed = 0;
  float leftSpeed = 0, rightSpeed = 0;
  int leftPWM = 0, rightPWM = 0;
  float Kp=1, Ki=0, Kd=0;
};

#endif
