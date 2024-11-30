#include "Alarm.h"
#include <Arduino.h>
// Function to set RGB color
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void redAlarm() {
  // Set color to Red
  setColor(255, 0, 0);
  delay(150);

  // Turn off
  setColor(0, 0, 0);
  delay(300);
}

void yellowAlarm() {
  // Set color to Yellow
  setColor(255, 255, 0);
  delay(150);

  // Turn off
  setColor(0, 0, 0);
  delay(300);
}

void noiseStream() {
  // Generate noise on D8 pin
  for (int i = 0; i < 100; i++) {
    digitalWrite(noisePin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(noisePin, LOW);
    delayMicroseconds(100);
  }
}