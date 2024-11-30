#include "PDI.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(57600);  // Initialize Serial communication with PC
  LoraSerial.begin(9600);  // Initialize LoRa communication
  LoraSerial.setTimeout(1000);
  init_lora();  // Initialize LoRa configuration
  Serial.println("LoRaP2P initialization complete. Ready to receive data...");
}

void loop() {
  // put your main code here, to run repeatedly:
  LOOP();
}
