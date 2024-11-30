#ifndef Lora_H
#define Lora_H

#include <SoftwareSerial.h>
#include <rn2xx3.h>


extern SoftwareSerial LoraSerial;  // Declare the LoraSerial object
extern rn2xx3 myLora;  // Declare the myLora object

void lora_autobaud();
int wait_for_ok();
void init_lora();
void send_lora_command(String command);
void initialize_radio();
String getLoraWAN_Downlink();

#endif 