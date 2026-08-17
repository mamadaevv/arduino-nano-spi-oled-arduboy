# Arduino Nano — Arduboy-порт (I²C SSD1306)

DIY-консоль на **Arduino Nano (ATmega328p)**, гоняющая **игры из публичного
каталога Arduboy** без переписывания кода.

## Что используем
- **Библиотека:** [`harbaum/Arduboy2`](https://github.com/harbaum/Arduboy2) —
  форк Arduboy2, портированный под Nano. При сборке под `atmega328p`
  автоматически включает ветку `SLIMBOY` (I²C-дисплей, пины под 328p).
- **Дисплей:** I²C **SSD1306** 128×64, адрес **0x3C** (4 пина: GND/VCC/SCL/SDA).
- **FQBN:** `arduino:avr:nano:cpu=atmega328old`

> ⚠️ Стандартная `Arduboy2` (из Library Manager) и пакет Mr.Blinky
> `Arduboy-homemade` — **НЕ подходят**: они заточены под ATmega32U4
> (Leonardo/Micro/ProMicro). На 328p не собираются.

## Распиновка (либа не трогается)
| Компонент | Nano pin | Примечание |
|-----------|----------|------------|
| OLED GND   | GND      | |
| OLED VCC   | 5V (или 3.3V, если модуль только 3.3В) | |
| OLED SCL   | **A5**   | I²C clock |
| OLED SDA   | **A4**   | I²C data |
| UP         | **A3**   | active LOW |
| DOWN       | **D2**   | active LOW |
| LEFT       | **A1**   | active LOW |
| RIGHT      | **D3**   | active LOW |
| A          | **D4**   | active LOW |
| B          | **A2**   | active LOW |
| Buzzer +   | **D9**   | PWM (Timer1 OC1A) |
| Buzzer +   | **D11**  | PWM (Timer2 OC2A) — двухголосый пьезо |
| Buzzer −   | —        | между D9 и D11, GND не нужен |

> Двухпроводной бузер (D9↔D11) даёт двухголосие + в 2 раза громче, чем
> одноконцевой на GND.

## Установка игры — одной командой
Скрипт `flash_game.cmd` сам ставит либу-порт (если нет), собирает и прошивает:

```bat
flash_game.cmd                 :: ArduBreakout на COM18
flash_game.cmd Tetris COM18    :: другая игра
flash_game.cmd ArduBreakout COM18 build  :: только собрать (HEX в build/)
```

Требования: `arduino-cli.exe` в PATH (или задай `set ARDUBIN=путь`),
Nano подключён и виден в `arduino-cli board list`.

## Проверено на железе
- ✅ `ArduBreakout` собирается (11982 байт / 39% flash, 1281 байт / 62% RAM)
  и прошивается на Nano, играет, кнопки + звук работают.
- Игры из каталога (примеры либы) идут «как есть» — правок либы не нужно.

## Прошивка вручную (без скрипта)
```bat
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old <путь к скетчу>
arduino-cli upload   -p COM18 --fqbn arduino:avr:nano:cpu=atmega328old <путь к скетчу>
```

## Gotcha
- Экран **должен быть SSD1306 @ 0x3C**. Если модуль — SH1106 или 0x3D,
  поедет только после правки `lcdBootProgram` в `src/Arduboy2Core.cpp`.
- Плата не видна в `board list` → CH340 отвалился: переткни USB / другой порт.
