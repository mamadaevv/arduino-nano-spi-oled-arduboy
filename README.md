# Arduino Nano — 7-pin SPI SSD1306 OLED + Arduboy buttons

DIY Arduboy-clone wiring for an Arduino Nano (CH340 clone, `atmega328old`) with a
7-pin SPI OLED module (**SSD1306**, 128×64, **3.3V** — no onboard regulator, feed 3.3V not 5V) and a 6-button Arduboy-style keypad.

## Wiring

### OLED (7-pin SPI module, R3 soldered → SPI mode, not I2C)
| Module pin | Nano pin |
|------------|----------|
| D0 (SCL)   | D13      |
| D1 (SDA)   | D11      |
| RES        | D6       |
| DC         | D4       |
| CS         | D10 |
| VCC        | 3.3V     |
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

## U8g2 offset note

The module is **SSD1306** (confirmed: a detector sketch built with the SSD1306
driver lit a clean white screen; an SH1106 driver would have stayed black). The
controller's default x-offset in U8g2 (`u8x8_d_ssd1306_128x64_noname.c`, line 353)
is currently `0` (was `2` originally) and the screen is clean with it. Line 473
(SH1106 driver, a different chip) was also changed to `0` by mistake earlier — it
should be reverted to `2` since this is not an SH1106 module.

If you ever connect a real SH1106 display, revert line 473 back to `2`.

> ⚠️ **ATmega328 (Nano) gotcha:** D12 = MISO. HW_SPI (4W_HW_SPI) forces MISO to
> INPUT, so a CS on D12 can never go LOW → black screen. **Use 4W_SW_SPI**
> (bit-bang) instead, then CS=D12 works as a plain GPIO:
> `U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, 13, 11, OLED_CS, OLED_DC, OLED_RESET);`

## Build & flash

```bash
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old SPI_OLED_test
arduino-cli upload  -p COM8  --fqbn arduino:avr:nano:cpu=atmega328old SPI_OLED_test
```

(SRAM usage is ~84% with U8g2 full-buffer — that is just a warning, it runs fine.)

### Buzzer (D5, passive piezo, mono)
A passive piezo buzzer is wired to **D5** (canonical Arduboy audio pin, mono —
D13 is used by SPI clock). Plays a short startup melody on boot and a short beep
(880 Hz, 80 ms) on each button press edge. Driven with the `tone()` / `noTone()`
Arduino API (PWM).

| Part  | Nano pin |
|-------|----------|
| Buzzer+ | D5      |
| Buzzer- | GND     |

> If you have an *active* (self-driving) piezo instead of a passive one, replace
> `tone()`/`noTone()` with `digitalWrite(BUZZER, HIGH/LOW)`.

## Files

- `SPI_OLED_test/SPI_OLED_test.ino` — current combined sketch: white-screen
  verify + live button-state display on the OLED.
- `button_test.ino` — standalone button test (OLED list, `[X]` when pressed).
- `buzzer_test.ino` — passive piezo on D5: startup melody + beep on button press.
- `vibration_test.ino` — 3-pin vibration motor on D9 + passive piezo on D5. OLED shows
  a two-column status screen: left = 6 buttons, right = Buzzer/Vibro/Led indicators.
  Holding a button keeps buzzer + vibro on continuously; release turns them off after
  the timeout. Screen header: `SPI SSD1306 128x64`.
- `WIRING.txt` — pinout diagram (canonical Arduboy layout).

## Pinout diagram (canonical Arduboy)

```
                 Arduino Nano (atmega328, 3.3V OLED)
                 ┌───────────────┐
         RES ────┤ D6        D12 ├──── CS
         DC  ────┤ D4        D11 ├──── D1 (MOSI/SDA)
         GND ────┤ GND       D13 ├──── D0 (SCL)
         VCC ────┤ 3.3V          │
                 │               │
         BUZ ────┤ D5            │
         VIB ────┤ D9            │
                 │               │
         UP  ────┤ A0        A1  ├──── RIGHT
         DOWN ───┤ A3        A2  ├──── LEFT
         A   ────┤ D7        D8  ├──── B
                 └───────────────┘

  OLED 7-pin SPI: VCC=3.3V, GND, D0=SCL→D13, D1=SDA→D11, RES→D6, DC→D4, CS→D12
  Buttons (active LOW): UP=A0 DOWN=A3 LEFT=A2 RIGHT=A1 A=7 B=8
  Buzzer: D5 (mono).  Vibro: D9 (3-pin module).
```

## Notes

- Buzzer on D9 is dead → startup indication uses the onboard LED (D13), 3 blinks.
- I2C scanner does not see this module (it is SPI, not I2C).
- Arduboy2 library targets SSD1306, so off-the-shelf Arduboy sketches should work
  Arduboy sketches (not yet verified).
