#ifndef GAME_H
#define GAME_H
#include <LITTLEFS.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Constants.h"
#include "Joystick.h"

#define MAX_SCORES 101   // максимум запоминаемых результатов

struct Coord {
  uint8_t x;
  uint8_t y;
};

struct Player {
  uint8_t size;
  Coord position;
  Coord previousPositions[4];
};

struct Target {
  Coord position;
  uint8_t size;
  int expirationTime;
  bool visible;
};

class Game
{
public:
  Game();
  
  bool loadScores();    // загрузка из файла
  bool saveScores(); 
  void sortScores();     // сохранение в файл
  void init(Adafruit_SSD1306 _display, Joystick _joystick);
  int8_t tick();
  void reset();
  void resume();

  uint8_t score;
  uint8_t trs_score;

  // Новое:
  int scores[MAX_SCORES];  // массив всех последних результатов
  uint8_t scoreCount;      // сколько уже сохранено

  void saveScore();        // сохраняет текущий результат в список

private:
  Adafruit_SSD1306 display;
  Joystick joystick;
  uint8_t screenWidth;
  uint8_t screenHeight;
  long startTime;
  long elapsedTime;
  long lastTickTime;
  Player player;
  Target targets[3];

  void render();
  void renderScore();
  void renderTimer();
  void renderInput();
  bool outOfTime();
  Target newRandomTarget();
};

#endif
