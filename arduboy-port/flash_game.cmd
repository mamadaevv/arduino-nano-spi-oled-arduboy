@echo off
REM ============================================================
REM  flash_game.cmd  —  Arduino Nano (ATmega328p) Arduboy-port
REM  Одной командой: ставит либу-порт, собирает и прошивает игру.
REM
REM  Использование:
REM    flash_game.cmd                 -> ArduBreakout на COM18
REM    flash_game.cmd Tetris COM18    -> игра "Tetris" на COM18
REM    flash_game.cmd ArduBreakout COM18 build   -> только сборка (без прошивки)
REM
REM  Требования:
REM    - arduino-cli.exe в PATH (или задай ARDUBIN)
REM    - libs папка Arduino:  %USERPROFILE%\Documents\Arduino\libraries
REM    - подключённый Nano (CH340) на нужном COM
REM ============================================================
setlocal EnableDelayedExpansion

set GAME=%1
if "%GAME%"=="" set GAME=ArduBreakout
set COM=%2
if "%COM%"=="" set COM=COM18
set MODE=%3
if "%MODE%"=="" set MODE=flash

set ARDUBIN=%ARDUBIN%
if "%ARDUBIN%"=="" set ARDUBIN=arduino-cli.exe

set LIBDIR=%USERPROFILE%\Documents\Arduino\libraries
set PORTLIB=%LIBDIR%\Arduboy2
set FQBN=arduino:avr:nano:cpu=atmega328old

echo [1/4] Проверка либы harbaum/Arduboy2 (Nano-port)...
if not exist "%PORTLIB%\src\Arduboy2Core.cpp" (
  echo   либа не найдена, качаю с GitHub...
  powershell -NoProfile -Command ^
    "$u='https://github.com/harbaum/Arduboy2/archive/refs/heads/master.zip';" ^
    "$t=Join-Path $env:TEMP 'arduboy2.zip';" ^
    "Invoke-WebRequest -Uri $u -OutFile $t -TimeoutSec 120;" ^
    "Expand-Archive -Path $t -DestinationPath (Join-Path $env:TEMP 'arduboy2') -Force;" ^
    "Remove-Item -Recurse -Force '%PORTLIB%' -ErrorAction SilentlyContinue;" ^
    "Copy-Item -Recurse (Join-Path $env:TEMP 'arduboy2\Arduboy2-master') '%PORTLIB%';" ^
    "Write-Host '   либа установлена'"
) else (
  echo   либа на месте: %PORTLIB%
)

set SKETCH=%PORTLIB%\examples\%GAME%
if not exist "%SKETCH%" (
  echo [ОШИБКА] нет примера "%GAME%" в %PORTLIB%\examples
  echo          доступно: ArduBreakout BeepDemo Buttons HelloWorld PlayTune RGBled SetSystemEEPROM VidTest
  exit /b 1
)

echo [2/4] Сборка "%GAME%" под %FQBN% ...
%ARDUBIN% compile --fqbn %FQBN% "%SKETCH%"
if errorlevel 1 (
  echo [ОШИБКА] сборка не удалась
  exit /b 1
)

if /i "%MODE%"=="build" (
  echo [готово] только сборка. HEX в build/ папке примера.
  exit /b 0
)

echo [3/4] Прошивка на %COM% ...
%ARDUBIN% upload -p %COM% --fqbn %FQBN% "%SKETCH%"
if errorlevel 1 (
  echo [ОШИБКА] прошивка не удалась (плата на другом COM? не видна в board list?)
  exit /b 1
)

echo [4/4] Готово! "%GAME%" залит на Nano (%COM%). Перезагрузка -> игра.
endlocal
