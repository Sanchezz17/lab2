#include "LedControl.h"
#include <Servo.h>
#include "button.h"

#define PIN_BUTTON 52

Servo servo;
int servoSpeed = 0;
const int maxServoDelay = 10;
const int minServoDelay = 2;

Button button(PIN_BUTTON);

const int boardSize = 8;
const int maxPointsCount = 64;
LedControl ledDisplay = LedControl(26, 22, 24, 1);  // (DIN, CLK, CS, Количество дисплеев)

const long long maxTime = 1000;

struct Point {
    int x;
    int y;
};

void setup() {
    servo.attach(9);
    ledDisplay.shutdown(0, false);
    ledDisplay.setIntensity(0, 10);
    ledDisplay.clearDisplay(0);
    Serial.begin(115200);
}

void loop() {
    int currentPower = 0;
    if (button.wasPressed())
    {
      long long beginTime = millis();
      while (button.isHolded())
      {
         long long holdingTime = millis() - beginTime;
         currentPower = convertIntoPercents(holdingTime, maxTime);
         updatePowerOnDisplay(currentPower);
         Serial.println(currentPower);
      }
      golfShoot(currentPower);
    }
    toInitialPosition();
}

void golfShoot(int currentPower)
{
    servoSpeed = convertFromPercents(currentPower, maxServoDelay);
    int currentDelay = max(minServoDelay, maxServoDelay - servoSpeed);
    Serial.print("servo");
    Serial.println(servoSpeed);
    for(int i=90; i<=125; i++)
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

void drawPoint(Point point)
{
    ledDisplay.setLed(0, boardSize - 1 - point.y, point.x, true);
}

void updatePowerOnDisplay(int currentPower)
{
    int pointsCount = convertFromPercents(currentPower, maxPointsCount);
    for (int i = 0; i < pointsCount; i++)
    {
        Point point;
        point.x = i / boardSize;
        point.y = i % boardSize;
        drawPoint(point);
    }
}

void toInitialPosition()
{
    servo.write(90);
    ledDisplay.clearDisplay(0);
}

int convertIntoPercents(long long currentValue, long long maxValue) // (текущее значение, значение для 100%)
{
    int percents = (currentValue * 100) / maxValue;
    return min(percents, 100);
}


int convertFromPercents(int percent, int maxValue) // (текущее значение в процентах, значение для 100%)
{
    return floor(maxValue * percent / 100);
}
