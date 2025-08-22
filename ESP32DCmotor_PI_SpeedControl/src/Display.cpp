#include "Display.h"

bool DisplayUI::begin(uint8_t addr, int16_t w, int16_t h) {
  (void)w; (void)h;
  if (!d.begin(SSD1306_SWITCHCAPVCC, addr)) return false;
  d.clearDisplay(); d.setTextSize(1); d.setTextColor(WHITE); d.display();
  return true;
}

void DisplayUI::setPage(UiPage p) { current = p; }
void DisplayUI::setStatus(float tgtSpeed, float lSpd, float rSpd, int lPWM, int rPWM) {
  targetSpeed = tgtSpeed; leftSpeed = lSpd; rightSpeed = rSpd; leftPWM = lPWM; rightPWM = rPWM;
}
void DisplayUI::setPid(float kp, float ki, float kd) { Kp=kp; Ki=ki; Kd=kd; }

void DisplayUI::drawStatus() {
  d.setCursor(0,0);   d.print("Target: "); d.print(targetSpeed,1);
  d.setCursor(0,10);  d.print("LPWM: "); d.print(leftPWM);
  d.setCursor(64,10); d.print("RPWM: "); d.print(rightPWM);
  d.setCursor(0,20);  d.print("Lspd: "); d.print(leftSpeed,1);
  d.setCursor(64,20); d.print("Rspd: "); d.print(rightSpeed,1);
  d.setCursor(0,54);  d.print("Pg0:Status  Pg1:Enc  Pg2:PID");
}

void DisplayUI::drawEncoder() {
  d.setCursor(0,0);   d.print("ENC SPEED (cnt/s)");
  d.setCursor(0,14);  d.print("L: "); d.print(leftSpeed,1);
  d.setCursor(64,14); d.print("R: "); d.print(rightSpeed,1);
  d.setCursor(0,54);  d.print("Pg1");
}

void DisplayUI::drawPid() {
  d.setCursor(0,0);  d.print("Speed PID");
  d.setCursor(0,14); d.print("Kp: "); d.print(Kp,3);
  d.setCursor(0,24); d.print("Ki: "); d.print(Ki,3);
  d.setCursor(0,34); d.print("Kd: "); d.print(Kd,3);
  d.setCursor(0,54); d.print("Pg2");
}

void DisplayUI::render() {
  d.clearDisplay();
  switch (current) {
    case PAGE_STATUS:  drawStatus();  break;
    case PAGE_ENCODER: drawEncoder(); break; // **第二页**显示编码器速度
    case PAGE_PID:     drawPid();     break;
  }
  d.display();
}
