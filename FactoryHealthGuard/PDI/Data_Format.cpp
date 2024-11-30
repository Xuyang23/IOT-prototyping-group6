#include "Data_Format.h"

float hexToFloat(String hexString) {
  union {
    uint32_t i;
    float f;
  } value;

  value.i = (uint32_t)strtoul(hexString.c_str(), NULL, 16);
  return value.f;
}

// Function to convert hexadecimal string to float array
void hexToFloatArray(String hexString, float floatArray[], int &size) {
  // Use a union to convert between byte array and float
  union {
    uint32_t i;
    float f;
  } value;

  size = 0;  // Reset size to zero for counting the number of floats

  // Each float is represented by 8 hex characters (4 bytes)
  for (int i = 0; i < hexString.length(); i += 8) {
    if (i + 8 <= hexString.length()) {  // Ensure there are enough characters left
      // Extract 8 characters from the hex string
      String floatHex = hexString.substring(i, i + 8);

      // Convert the hex string to an integer (4 bytes)
      value.i = (uint32_t)strtoul(floatHex.c_str(), NULL, 16);

      // Store the float value into the array
      floatArray[size++] = value.f;
    }
  }
}

String asciiToHex(String asciiString) {
  String hexString = "";
  for (int i = 0; i < asciiString.length(); i++) {
    char c = asciiString.charAt(i);
    String hexChar = String(c, HEX);
    if (hexChar.length() == 1) {
      hexChar = "0" + hexChar; // Ensure two-digit hex values
    }
    hexString += hexChar;
  }
  return hexString;
}

uint8_t hexCharToValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0; // 非法字符处理
}


void hexStringToByteArray(String hexString, uint8_t* byteArray, size_t byteArraySize) {
  size_t hexLength = hexString.length();
  size_t byteIndex = 0;

  for (size_t i = 0; i < hexLength; i += 2) {
    if (byteIndex >= byteArraySize) break; // 防止数组越界
    byteArray[byteIndex++] = (hexCharToValue(hexString[i]) << 4) | hexCharToValue(hexString[i + 1]);
  }
}

// Function to convert a float array to a hexadecimal string
String floatArrayToHex(float floatArray[], int size) {
  String hexString = "";

  // Use a union to convert between float and byte array
  union {
    float f;
    uint8_t bytes[4];
  } value;

  for (int i = 0; i < size; i++) {
    value.f = floatArray[i];  // Store the float value into the union

    // Convert each byte of the float to a two-character hexadecimal string
    for (int j = 3; j >= 0; j--) {  // Traverse from most significant to least significant byte
      if (value.bytes[j] < 0x10) {
        hexString += "0";  // Add leading zero for single-digit hex numbers
      }
      hexString += String(value.bytes[j], HEX);
    }
  }

  return hexString;
}
