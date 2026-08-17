# Arduino Nano — 7-pin SPI SH1106 OLED + Arduboy buttons

DIY Arduboy-clone wiring for an Arduino Nano (CH340 clone, `atmega328old`) with a
7-pin SPI OLED module (SH1106, 128×64) and a 6-button Arduboy-style keypad.

## Wiring

### OLED (7-pin SPI module, R3 soldered → SPI mode, not I2C)
| Module pin | Nano pin |
|------------|----------|
| D0 (SCL)   | D13      |
| D1 (SDA)   | D11      |
| RES        | D5       |
| DC         | D4       |
| CS         | D6       |
| VCC        | 5V       |
| GND        | GND      |

> CS must go to a driven pin (D6), not GND — otherwise chip-select is not
> toggled and the display may not initialize correctly.

### Buttons (Arduboy layout, active LOW)
Each button: one side to the pin, other side to GND. Pins are set to
`INPUT_PULLUP`, so a pressed button reads `LOW`.

| Button | Nano pin |
|--------|----------|
| UP     | A0       |
| DOWN   | A3       |
| LEFT   | A2       |
| RIGHT  | A1       |
| A      | 7        |
| B      | 8        |

## Required patch: U8g2 SH1106 x-offset

The 7-pin SH1106 module shows 2 columns of garbage on the left and does not
use the full 128-px width until you fix the driver's default x-offset.

File: `U8g2/src/clib/u8x8_d_ssd1306_128x64_noname.c`
SH1106 128×64 driver — around **line 473**:

```c
/* default_x_offset = */ 2,   →   0,
```

After this patch the full screen is clean white with no garbage columns.

⚠️ **Warning:** the same patch also changes **line 353** (SSD1306 128×64 driver)
from `2` to `0`. If you later connect a 4-pin I2C SSD1306 display with the same
U8g2 install, revert that line back to `2`, otherwise the image will be shifted.

## Build & flash

```bash
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old SPI_OLED_test
arduino-cli upload  -p COM8  --fqbn arduino:avr:nano:cpu=atmega328old SPI_OLED_test
```

(SRAM usage is ~84% with U8g2 full-buffer — that is just a warning, it runs fine.)

### Buzzer (D3, passive piezo)
A passive piezo buzzer is wired to **D3** (the canonical Arduboy audio pin). Plays
a short startup melody on boot and a short beep (880 Hz, 80 ms) on each button
press edge. Driven with the `tone()` / `noTone()` Arduino API (PWM).

| Part  | Nano pin |
|-------|----------|
| Buzzer+ | D3      |
| Buzzer- | GND     |

> If you have an *active* (self-driving) piezo instead of a passive one, replace
> `tone()`/`noTone()` with `digitalWrite(BUZZER, HIGH/LOW)`.

## Files

- `SPI_OLED_test/SPI_OLED_test.ino` — current combined sketch: white-screen
  verify + live button-state display on the OLED.
- `button_test.ino` — standalone button test (OLED list, `[X]` when pressed).
- `buzzer_test.ino` — passive piezo on D3: startup melody + beep on button press.

## Notes

- Buzzer on D9 is dead → startup indication uses the onboard LED (D13), 3 blinks.
- I2C scanner does not see this module (it is SPI, not I2C).
- Arduboy2 library targets SSD1306; SH1106 may need adaptation for off-the-shelf
  Arduboy sketches (not yet verified).
