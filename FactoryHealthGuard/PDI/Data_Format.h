#ifndef Data_Foramt_H
#define Data_Foramt_H

#include <stdlib.h>
#include <Arduino.h>

uint8_t hexCharToValue(char c);
float hexToFloat(String hexString);
void hexToFloatArray(String hexString, float floatArray[], int &size);
void hexStringToByteArray(String hexString, uint8_t* byteArray, size_t byteArraySize);

String floatArrayToHex(float floatArray[], int size);
String asciiToHex(String asciiString);

#endif 