#include <U8g2lib.h>
#include <SPI.h>

#define LED 13
#define OLED_DC    4
#define OLED_RESET 6
#define OLED_CS    10
#define BUZZER     5
#define VIB        9

// Arduboy buttons (active LOW)
#define BTN_UP    A0
#define BTN_DOWN  A3
#define BTN_LEFT  A2
#define BTN_RIGHT A1
#define BTN_A     7
#define BTN_B     8

// HW_SPI with CS=D10 (AVR SS pin; not blocked by HW-SPI like D12=MISO)
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, OLED_CS, OLED_DC, OLED_RESET);

const char* names[6] = {"UP", "DOWN", "LEFT", "RIGHT", "A", "B"};
const uint8_t pins[6] = {BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B};
bool prev[6] = {false, false, false, false, false, false};

// status indicators: time at which the peripheral turns OFF
unsigned long buzOffAt = 0;
unsigned long vibOffAt = 0;

#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_C6 1047
struct { int f; int d; } melody[] = {
  {NOTE_C5, 150}, {NOTE_E5, 150}, {NOTE_G5, 150}, {NOTE_C6, 300},
  {NOTE_G5, 150}, {NOTE_C6, 400}
};

// non-blocking triggers: start the effect, expire later in loop()
void triggerBeep(int freq, int dur) {
  tone(BUZZER, freq);
  buzOffAt = millis() + dur;
}
void triggerVib(int ms) {
  digitalWrite(VIB, HIGH);
  vibOffAt = millis() + ms;
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 6; i++) pinMode(pins[i], INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(VIB, OUTPUT);
  digitalWrite(VIB, LOW);
  u8g2.begin();
  for (int i = 0; i < 3; i++) { digitalWrite(LED, HIGH); delay(150); digitalWrite(LED, LOW); delay(150); }

  // startup: melody + vibrate (blocking OK here, screen not drawn yet)
  for (int i = 0; i < 6; i++) {
    triggerBeep(melody[i].f, melody[i].d);
    triggerVib(melody[i].d);
    delay(melody[i].d + 40);
    noTone(BUZZER);
    digitalWrite(VIB, LOW);
  }
  Serial.println("VIB+BUZZER+BTN READY");
}

void loop() {
  unsigned long now = millis();
  bool buzOn = (now < buzOffAt);
  bool vibOn = (now < vibOffAt);
  bool ledOn = (digitalRead(LED) == HIGH);

  // expire effects
  if (!buzOn) noTone(BUZZER);
  if (!vibOn) digitalWrite(VIB, LOW);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x7_tf);

  // header: display type / resolution
  u8g2.setCursor(0, 8);
  u8g2.print("SPI SSD1306 128x64");

  // left column: buttons
  for (int i = 0; i < 6; i++) {
    bool pressed = (digitalRead(pins[i]) == LOW);
    char buf[16];
    sprintf(buf, "%-5s [%c]", names[i], pressed ? 'X' : ' ');
    u8g2.setCursor(0, 20 + i * 7);
    u8g2.print(buf);
    if (pressed) { triggerBeep(880, 80); triggerVib(200); }  // continuous while held
    prev[i] = pressed;
  }

  // right column: peripherals
  char rbuf[16];
  sprintf(rbuf, "Buzzer [%c]", buzOn ? 'X' : ' ');
  u8g2.setCursor(70, 20); u8g2.print(rbuf);
  sprintf(rbuf, "Vibro  [%c]", vibOn ? 'X' : ' ');
  u8g2.setCursor(70, 28); u8g2.print(rbuf);
  sprintf(rbuf, "Led    [%c]", ledOn ? 'X' : ' ');
  u8g2.setCursor(70, 36); u8g2.print(rbuf);

  u8g2.sendBuffer();
  delay(40);
}
