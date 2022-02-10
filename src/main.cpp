#include <arduino.h>
#include "eventbutton.h"

const int buttonPin = 0;
const int ledPin = 1;

void setup()
{
    Serial.begin(115200);
    pinMode(buttonPin, INPUT);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    Serial.println("setup");

}

void buttonCallback()
{
    Serial.println("buttonCallback");

    static int ledState = HIGH;
    digitalWrite(ledPin, ledState);
    ledState = !ledState;
}

embedded::EventButton eventButton(500, buttonCallback, millis);

void loop()
{
    eventButton.update(digitalRead(buttonPin));
}