# Pac-Man for ESP32-S3

A small Pac-Man-inspired graphics demo for an **ESP32-S3**, a **320×240 ST7789 TFT display**, and a **two-axis analog joystick**. Move Pac-Man around the screen with the joystick; the game draws a simple maze background, dots, and a direction-aware Pac-Man sprite.

> This is a lightweight demo rather than a complete Pac-Man clone: it currently has no wall collision, ghosts, scoring, or dot collection.

## Hardware

- ESP32-S3 development board
- ST7789 SPI TFT display (240×320, used in landscape mode)
- Analog joystick module with `X`, `Y`, and optional push-button (`SW`) connections

### Wiring

| Device signal | ESP32-S3 GPIO |
| --- | ---: |
| TFT MOSI | 11 |
| TFT SCLK | 12 |
| TFT CS | 10 |
| TFT DC | 9 |
| TFT RST | 14 |
| TFT backlight | 13 |
| Joystick X | 1 |
| Joystick Y | 2 |
| Joystick switch | 8 |

Connect the display and joystick power and ground to suitable **3.3 V** and **GND** pins on your board. The joystick switch is configured with `INPUT_PULLUP`; connect it between GPIO 8 and ground. It is initialized but not yet used by the game.

## Software requirements

The code targets the Arduino framework for ESP32. Install:

1. The **ESP32 by Espressif Systems** board package.
2. These libraries from Arduino Library Manager:
   - `Adafruit GFX Library`
   - `Adafruit ST7789 and ST7735 Library`

## Build and upload

1. Open the project in an Arduino-compatible ESP32 workflow (Arduino IDE or PlatformIO).
2. Compile `main.c` as **C++**—for the Arduino IDE, rename or copy it to a sketch file such as `pacman_esp32.ino`.
3. Select your ESP32-S3 board and serial port.
4. Install the required libraries, then build and upload.
5. On startup, leave the joystick untouched for about one second while it calibrates its center position.

## Controls and configuration

Tilt the joystick to move Pac-Man. The program automatically selects the dominant axis and continues moving in the last direction selected.

If movement does not match the joystick direction, adjust these constants near the top of `main.c`:

```cpp
const bool SWAP_AXES = false;
const bool INVERT_X  = false;
const bool INVERT_Y  = false;
```

You can also tune `DEADZONE` to reduce joystick drift and `SPEED` to change movement speed.

## License

No license has been specified for this project yet.
