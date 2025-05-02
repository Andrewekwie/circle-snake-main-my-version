#include "Joystick.h"

Joystick::Joystick()
{}

Joystick::Joystick(uint8_t x_pin, uint8_t y_pin, uint8_t sw_pin)
{
  xPin = x_pin;
  yPin = y_pin;
  swPin = sw_pin;
  rotation = ROTATION_0;
  xMidpoint = 1255;
  yMidpoint = 2803;
  maxValue = 4095;
}

void Joystick::init()
{
  pinMode(swPin, INPUT_PULLUP);
  xInitValue = analogRead(xPin);
  yInitValue = analogRead(yPin);
  buttonValue = digitalRead(swPin);
  buttonPreviousValue = buttonValue;
}

void Joystick::setRotation(Rotation _rotation)
{
  rotation = _rotation;
}

void Joystick::update()
{
  int rawX = analogRead(xPin);
  int rawY = analogRead(yPin);
  int scaledX = 0;
  int scaledY = 0;


  if(abs(xInitValue-rawX) >= 50) {

    if(rawX > xInitValue)
      scaledX = map(rawX, xInitValue, 4095, 0, 100); 
    else
      scaledX = map(rawX, 0, xInitValue, -100, 0); 
  }

  if(abs(yInitValue-rawY) >= 35) {
    if(rawY > yInitValue)
      scaledY = map(rawY, yInitValue, 4095, 0, 100); 
    else
      scaledY = map(rawY, 0, yInitValue, -100, 0); 
  }
  

  switch(rotation){
    case ROTATION_0:
      xValue = scaledX;
      yValue = scaledY;
      break;
    case ROTATION_90:
      xValue = -1 * scaledY;
      yValue = -1 * scaledX;
      break;
    case ROTATION_180:
      xValue = scaledY;
      yValue = scaledX;
      break;
    case ROTATION_270:
      xValue = -1 * scaledY;
      yValue = -1 * scaledX;
      break;
  }


  buttonPreviousValue = buttonValue;
  buttonValue = digitalRead(swPin);


}

int Joystick::getX()
{
  return xValue;
}

int Joystick::getY()
{
  return yValue;
}

bool Joystick::buttonPressed()
{
  return buttonValue == 0 && buttonValue != buttonPreviousValue;
}

Direction Joystick::direction()
{
  if(xValue < -40)
    return LEFT;
  else if(xValue > 40)
    return RIGHT;
  else if(yValue > 40)
    return UP;
  else if(yValue < -40)
    return DOWN;
  else
    return NONE;
}
