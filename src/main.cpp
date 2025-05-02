#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LITTLEFS.h>
#include <FS.h>
#include "Constants.h"
#include "Joystick.h"
#include "Game.h"
#include "Menu.h"
int scrollIndex = 0;

#include <LittleFS.h>
#define OLED_ADDR 0x3C

const uint8_t OLED_WIDTH = 128;
const uint8_t OLED_HEIGHT = 64;

// Joystick pins
const uint8_t X_PIN = 33; 
const uint8_t Y_PIN = 32; 
const uint8_t SW_PIN = 25;
const uint8_t pin = 13;

const bool DEBUG = false;
uint8_t menuItem = 0;
const uint8_t buzzerPin = 13;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
Joystick joystick(X_PIN, Y_PIN, SW_PIN);
Game game;
View currentView;

void debug(String str)
{
  if(DEBUG)
    Serial.println(str);
}

void setup()
{
  Serial.begin(115200); 

  if (!LittleFS.begin()) {
    Serial.println("LITTLEFS Mount Failed");
  } else {
    game.loadScores();  
  }



  pinMode(buzzerPin, OUTPUT);

  ledcSetup(0, 4000, 8);
  ledcAttachPin(buzzerPin, 0);




  
  Serial.begin(115200);
  randomSeed(analogRead(X_PIN));

  joystick.init();
  joystick.setRotation(ROTATION_90);

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();

  game.init(display, joystick);
  currentView = START_VIEW;
}

void loop()
{
   if (joystick.buttonPressed()) {
      ledcWrite(0, 40);
      delay(150);
      ledcWrite(0, 0);
    }

  
  display.clearDisplay();
  joystick.update();

  switch(currentView)
          {  case(DELETE_CONFIRM_VIEW):
        {
          static int warningStep = 0;

          display.fillScreen(SSD1306_BLACK);
          display.setTextColor(SSD1306_INVERSE);
          display.setTextSize(1);
          display.setCursor(10, 10);
          display.printf("WARNING %d/3", warningStep + 1);
          display.setCursor(10, 30);
          display.println("Delete all scores?");
          display.setCursor(10, 45);
          display.println("Press to confirm");

          if(joystick.buttonPressed()) {
            warningStep++;
            if(warningStep >= 3) {
              LittleFS.remove("/scores.txt");
              game.scoreCount = 0;
              scrollIndex = 0;
              currentView = START_VIEW;
              warningStep = 0;
            }
          } else if (joystick.direction() == LEFT || joystick.direction() == UP) {
            currentView = START_VIEW;
            warningStep = 0;
          }
          break;
        }
    
    
    
        case(SCORE_VIEW):
        {
          if(joystick.direction() == LEFT && menuItem < 1)
            menuItem++;
          if(joystick.direction() == RIGHT && menuItem > 0)
            menuItem--;
        
          debug(" - Score View >>>>>>>>>>>>>>>");
          display.fillScreen(SSD1306_BLACK);
          display.setTextColor(SSD1306_INVERSE);
          display.setTextSize(1);
          display.setCursor(0, 0);
          display.println("Top Scores:");
        
          for (int i = 0; i < 5 && (scrollIndex + i) < game.scoreCount; ++i) {
            display.setCursor(10, 10 + i * 10);
            display.printf("%d: %d", scrollIndex + i + 1, game.scores[scrollIndex + i]);
          }
        

          if (menuItem == 1) {

            display.fillRoundRect(0, 50, 128, 14, 4, SSD1306_WHITE);
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); 
          } else {

            display.fillRoundRect(0, 50, 128, 14, 4, SSD1306_BLACK);
            display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
          }
          

          display.setTextSize(1);
          display.setCursor((128 - 72) / 2, 54);
          display.println("Delete scores");
          if (joystick.buttonPressed()) {
            if(menuItem == 0){
              currentView = START_VIEW;
            }
            if(menuItem == 1){
              currentView = DELETE_CONFIRM_VIEW;
            }
          } else {
            if (joystick.direction() == DOWN && scrollIndex + 5 < game.scoreCount) {
              scrollIndex++;
            }
            if (joystick.direction() == UP && scrollIndex > 0) {
              scrollIndex--;
            }
          }
          break;
        }
    

    case(START_VIEW):
    {
      
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);

      display.fillRoundRect(13, 10, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 1)
        display.fillRoundRect(14, 11, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(46, 16);
      display.println("Play");

      display.fillRoundRect(13, 34, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 0)
        display.fillRoundRect(14, 35, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(32, 40);
      display.println("Last scores");

      if(joystick.buttonPressed())
      {
        if(menuItem == 0)
        {
          game.reset();
          currentView = PLAY_VIEW;
        }
        else if(menuItem == 1)
        {
          currentView = SCORE_VIEW;
        }
      }
      else
      {
        if(joystick.direction() == DOWN)
          menuItem = 1;
        if(joystick.direction() == UP)
          menuItem = 0;
      }
      break;
    }

    case(PLAY_VIEW):
    {
      

      if(game.tick() == -1)
      {
        currentView = GAME_OVER_VIEW;
        menuItem = 0;
      }
      else if(joystick.buttonPressed())
      {
        currentView = PAUSE_VIEW;
        menuItem = 0;
      }
      break;
    }

    case(PAUSE_VIEW):
    {
      
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);

      display.fillRoundRect(13, 10, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 1)
        display.fillRoundRect(14, 11, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(41, 16);
      display.println("Resume");

      display.fillRoundRect(13, 34, 100, 20, 4, SSD1306_WHITE);
      if(menuItem == 0)
        display.fillRoundRect(14, 35, 98, 18, 4, SSD1306_INVERSE);
      display.setTextSize(1);
      display.setCursor(47, 40);
      display.println("Quit");

      if(joystick.buttonPressed())
      {
        if(menuItem == 0)
        {
          game.resume();
          currentView = PLAY_VIEW;
        }
        else if(menuItem == 1)
        {
          game.reset();
          currentView = START_VIEW;
          menuItem = 0;
        }
      }
      else
      {
        if(joystick.direction() == DOWN)
          menuItem = 1;
        if(joystick.direction() == UP)
          menuItem = 0;
      }
      break;
    }

    case(GAME_OVER_VIEW):
    {
      
      display.fillScreen(SSD1306_BLACK);
      display.setTextColor(SSD1306_INVERSE);
      display.setTextSize(2);
      display.setCursor(10, 10);
      display.println("Game Over");
      display.setCursor(10, 30);
      display.printf("Score: %d", game.score);
      if(joystick.buttonPressed())
      {
        currentView = START_VIEW;
        game.reset();
      }
      break;
    }
  }

  display.display();
  delay(20);
}
