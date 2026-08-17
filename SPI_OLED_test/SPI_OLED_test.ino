#include <U8g2lib.h>
#include <SPI.h>

#define LED 13
#define OLED_DC    4
#define OLED_RESET 5
#define OLED_CS    6

// Arduboy buttons (active LOW)
#define BTN_UP    A0
#define BTN_DOWN  A3
#define BTN_LEFT  A2
#define BTN_RIGHT A1
#define BTN_A     7
#define BTN_B     8

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, OLED_CS, OLED_DC, OLED_RESET);

const char* names[6] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B"};
const uint8_t pins[6] = {BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) pinMode(pins[i], INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  u8g2.begin();
  for (int i = 0; i < 3; i++) { digitalWrite(LED, HIGH); delay(150); digitalWrite(LED, LOW); delay(150); }
  Serial.println("BUTTON TEST READY");
}

void loop() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setCursor(0, 10);
  u8g2.print("Button test (LOW=ON)");
  for (int i = 0; i < 6; i++) {
    bool pressed = (digitalRead(pins[i]) == LOW);
    char buf[16];
    sprintf(buf, "%-5s %s", names[i], pressed ? "[X]" : "[ ]");
    u8g2.setCursor(0, 24 + i * 8);
    u8g2.print(buf);
    if (pressed) { Serial.print(names[i]); Serial.println(" PRESSED"); }
  }
  u8g2.sendBuffer();
  delay(80);
}
