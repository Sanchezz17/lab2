#include "LedControl.h"
#include <Servo.h>
#include "button.h"

#define PIN_BUTTON 52
#define PIN_SERVO 9

Button button(PIN_BUTTON);

Servo servo;
int servoSpeed = 0;
int servoInitial = 90;
const int maxServoDelay = 10;
const int minServoDelay = 1;

const int boardSize = 8;
const int maxPointsCount = 64;
LedControl ledDisplay = LedControl(26, 22, 24, 1);  // (DIN, CLK, CS, Количество дисплеев)

const long long maxTime = 1000;

struct Point {
    int x;
    int y;
};

void setup() {
    servo.attach(PIN_SERVO);
    ledDisplay.shutdown(0, false);
    ledDisplay.setIntensity(0, 10);
    ledDisplay.clearDisplay(0);
}

void loop() {
    int currentPower = 0;
    if (button.wasPressed())
    {
      int iterationCount = 0;
      long long beginTime = millis();
      while (button.isHolded())
      {
         long long holdingTime = millis() - beginTime;
         currentPower = holdingTime / maxTime;
         iterationCount = currentPower;
         updatePowerOnDisplay(currentPower);
      }
      if (iterationCount % 2 == 0)
          currentPower = currentPower % 100;
      else
          currentPower = 100 - currentPower % 100; 
      golfShoot(currentPower % 100);
      toInitialPosition();
    }
}

void golfShoot(int currentPower)
{
    servoSpeed = currentPower * maxServoDelay;
    int currentDelay = max(minServoDelay, maxServoDelay - servoSpeed);
    for(int i=servoInitial; i<=125; i++)
    {
        long currentTime = millis();
        servo.write(i);
        while (millis() - currentTime <= currentDelay);
    }
    for(int i=125; i>=0; i--)
    {
        long currentTime = millis();
        servo.write(i);
        while (millis() - currentTime <= currentDelay);
    }
}

void drawPoint(Point point, bool value)
{
    ledDisplay.setLed(0, boardSize - 1 - point.y, point.x, value);
}

void updatePowerOnDisplay(int currentPower)
{
    int possiblePointsCount = maxPointsCount * currentPower;
    int pointsCount = possiblePointsCount % maxPointsCount;
    int iterationCount = possiblePointsCount / maxPointsCount;

    if (iterationCount % 2 == 0)
    {
      for (int i = 0; i < pointsCount; i++)
      {
          Point point;
          point.x = i / boardSize;
          point.y = i % boardSize;
          drawPoint(point, true);
      }
    }
    else
    {
      for (int i = 0; i < pointsCount; i++)
      {
          Point point;
          point.x = boardSize - 1 - i / boardSize;
          point.y = boardSize - 1 - i % boardSize;
          drawPoint(point, false);
      } 
    }
}

void toInitialPosition()
{
    for(int i=0; i<=90; i++)
    {
        servo.write(i);
        delay(15);
    }
    ledDisplay.clearDisplay(0);
}
