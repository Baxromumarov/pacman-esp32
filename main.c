#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ---------- DISPLAY PINS ----------
#define TFT_MOSI 11
#define TFT_SCLK 12
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST  14
#define TFT_BL   13

// ---------- JOYSTICK PINS ----------
#define JOY_X    1
#define JOY_Y    2
#define JOY_SW   8

// Your fixed joystick settings
const bool SWAP_AXES = false;
const bool INVERT_X  = false;
const bool INVERT_Y  = false;

const int SCREEN_W = 320;
const int SCREEN_H = 240;

const int DEADZONE = 500;
const int SPEED = 3;

Adafruit_ST7789 tft = Adafruit_ST7789(
  TFT_CS,
  TFT_DC,
  TFT_MOSI,
  TFT_SCLK,
  TFT_RST
);

int centerX = 2048;
int centerY = 2048;

int pacX = 160;
int pacY = 120;

int oldPacX = pacX;
int oldPacY = pacY;

enum Direction {
  DIR_CENTER,
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN
};

Direction currentDir = DIR_CENTER;
Direction lastMoveDir = DIR_RIGHT;

void readJoystickRaw(int &xVal, int &yVal) {
  xVal = analogRead(JOY_X);
  yVal = analogRead(JOY_Y);

  if (SWAP_AXES) {
    int temp = xVal;
    xVal = yVal;
    yVal = temp;
  }

  if (INVERT_X) xVal = 4095 - xVal;
  if (INVERT_Y) yVal = 4095 - yVal;
}

void calibrateJoystick() {
  long sumX = 0;
  long sumY = 0;

  for (int i = 0; i < 80; i++) {
    int xVal, yVal;
    readJoystickRaw(xVal, yVal);
    sumX += xVal;
    sumY += yVal;
    delay(5);
  }

  centerX = sumX / 80;
  centerY = sumY / 80;
}

Direction getDirection() {
  int xVal, yVal;
  readJoystickRaw(xVal, yVal);

  int dx = xVal - centerX;
  int dy = yVal - centerY;

  if (abs(dx) < DEADZONE && abs(dy) < DEADZONE) {
    return DIR_CENTER;
  }

  if (abs(dx) > abs(dy)) {
    if (dx < 0) return DIR_LEFT;
    return DIR_RIGHT;
  } else {
    if (dy < 0) return DIR_UP;
    return DIR_DOWN;
  }
}

void drawMazeBackground() {
  tft.fillScreen(ST77XX_BLACK);

  // simple border
  tft.drawRect(0, 0, SCREEN_W, SCREEN_H, ST77XX_BLUE);
  tft.drawRect(1, 1, SCREEN_W - 2, SCREEN_H - 2, ST77XX_BLUE);

  // simple test walls
  tft.drawRect(40, 40, 80, 20, ST77XX_BLUE);
  tft.drawRect(200, 40, 80, 20, ST77XX_BLUE);
  tft.drawRect(40, 180, 80, 20, ST77XX_BLUE);
  tft.drawRect(200, 180, 80, 20, ST77XX_BLUE);

  // dots
  for (int x = 20; x < 300; x += 20) {
    for (int y = 20; y < 220; y += 20) {
      tft.fillCircle(x, y, 2, ST77XX_WHITE);
    }
  }
}

void erasePacman(int x, int y) {
  // erase only Pac-Man area, not full screen
  tft.fillCircle(x, y, 16, ST77XX_BLACK);
}

void drawPacman(int x, int y, Direction dir) {
  tft.fillCircle(x, y, 14, ST77XX_YELLOW);

  // eye
  tft.fillCircle(x + 3, y - 6, 3, ST77XX_BLACK);

  // mouth direction
  if (dir == DIR_LEFT) {
    tft.fillTriangle(x, y, x - 15, y - 8, x - 15, y + 8, ST77XX_BLACK);
  } else if (dir == DIR_RIGHT || dir == DIR_CENTER) {
    tft.fillTriangle(x, y, x + 15, y - 8, x + 15, y + 8, ST77XX_BLACK);
  } else if (dir == DIR_UP) {
    tft.fillTriangle(x, y, x - 8, y - 15, x + 8, y - 15, ST77XX_BLACK);
  } else if (dir == DIR_DOWN) {
    tft.fillTriangle(x, y, x - 8, y + 15, x + 8, y + 15, ST77XX_BLACK);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  pinMode(JOY_SW, INPUT_PULLUP);

  analogReadResolution(12);

  tft.init(240, 320);
  tft.setRotation(1); // horizontal

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(30, 90);
  tft.println("Calibrating...");
  tft.setCursor(30, 120);
  tft.println("Don't touch joystick");

  delay(1000);
  calibrateJoystick();

  drawMazeBackground();
  drawPacman(pacX, pacY, lastMoveDir);
}

void loop() {
  Direction dir = getDirection();

  oldPacX = pacX;
  oldPacY = pacY;

  if (dir != DIR_CENTER) {
    currentDir = dir;
    lastMoveDir = dir;
  }

  if (currentDir == DIR_LEFT) {
    pacX -= SPEED;
  } else if (currentDir == DIR_RIGHT) {
    pacX += SPEED;
  } else if (currentDir == DIR_UP) {
    pacY -= SPEED;
  } else if (currentDir == DIR_DOWN) {
    pacY += SPEED;
  }

  // keep inside screen
  if (pacX < 16) pacX = 16;
  if (pacX > SCREEN_W - 16) pacX = SCREEN_W - 16;
  if (pacY < 16) pacY = 16;
  if (pacY > SCREEN_H - 16) pacY = SCREEN_H - 16;

  if (pacX != oldPacX || pacY != oldPacY) {
    erasePacman(oldPacX, oldPacY);
    drawPacman(pacX, pacY, lastMoveDir);
  }

  delay(25); // about 40 FPS
}