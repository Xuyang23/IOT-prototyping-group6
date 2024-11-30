#ifndef Alarm_H
#define Alarm_H

// Define pins for RGB
#define redPin  3  // D3 on Pro Mini
#define greenPin  4  // D4 on Pro Mini
#define bluePin  5  // D5 on Pro Mini
#define noisePin  8  // D8 for noise

void setColor(int red, int green, int blue);
void redAlarm();
void yellowAlarm();
void noiseStream();
#endif 