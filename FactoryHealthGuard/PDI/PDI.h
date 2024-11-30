#ifndef PDI_H
#define PDI_H

#include "Alarm.h"
#include "Data_Format.h"
#include "Lora.h"

#define sensor_num 3
extern float floatData[sensor_num];  // Declare the floatData array as an external variable
extern float Threshold[sensor_num];
extern bool alarm;

void LOOP();
void processReceivedData(int size);
void checkForAlarm(int size);
void triggerAlarm();
void sendDataOverLoRaWAN(float* floatData, int size);
void processDownlink(const String downlinkPayload);
#endif 