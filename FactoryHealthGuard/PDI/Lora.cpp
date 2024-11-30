#include "Lora.h"

SoftwareSerial LoraSerial(10, 11); // RX, TX
rn2xx3 myLora(LoraSerial);

void lora_autobaud() {
  String response = "";
  unsigned long startMillis = millis();
  while (response == "" && millis() - startMillis < 5000) {  // Timeout after 5 seconds
    delay(1000);
    LoraSerial.write((byte)0x00);
    LoraSerial.write(0x55);
    LoraSerial.println();
    LoraSerial.println("sys get ver");
    response = LoraSerial.readStringUntil('\n');
  }
  if (response == "") {
    Serial.println("Autobaud failed!");
  } else {
    Serial.println("Autobaud successful!");
  }
}

/*
 * This function blocks until the word "ok\n" is received on the UART,
 * or until a timeout of 3*5 seconds.
 */
int wait_for_ok() {
  String str = LoraSerial.readStringUntil('\n');
  if (str.indexOf("ok") == 0) {
    return 1;
  } else {
    return 0;
  }
}

void init_lora() {

    //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  LoraSerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  // Initialize LoRa module with configuration settings
  send_lora_command("sys get ver");  // Get firmware version
  send_lora_command("mac pause");  // Pause LoRaWAN
  
  send_lora_command("radio set mod lora");  // Set modulation to LoRa
  send_lora_command("radio set freq 869100000");  // Set frequency
  send_lora_command("radio set pwr 14");  // Set power
  send_lora_command("radio set sf sf7");  // Set spreading factor
  send_lora_command("radio set afcbw 41.7");  // Set auto frequency correction bandwidth
  send_lora_command("radio set rxbw 125");  // Set receiver bandwidth
  send_lora_command("radio set prlen 8");  // Set preamble length
  send_lora_command("radio set crc on");  // Enable CRC
  send_lora_command("radio set iqi off");  // Disable IQ inversion
  send_lora_command("radio set cr 4/5");  // Set coding rate
  send_lora_command("radio set wdt 60000");  // Set watchdog timer
  send_lora_command("radio set sync 12");  // Set sync word
  send_lora_command("radio set bw 125");  // Set bandwidth
  send_lora_command("radio rx 0");  // Command to start receiving LoRa data
}

void send_lora_command(String command) {
  // Send LoRa command and print response
  Serial.println(command);
  LoraSerial.println(command);
  if (wait_for_ok()) { // Wait for "ok" confirmation
    //Serial.println("Command executed successfully.");
  } else {
    Serial.println("Failed to execute command.");
  }
}

void initialize_radio()
{
  //reset rn2483
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  delay(500);
  digitalWrite(12, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  LoraSerial.flush();

  //Autobaud the rn2483 module to 9600. The default would otherwise be 57600.
  myLora.autobaud();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(myLora.hweui());
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;


  /*
   * ABP: initABP(String addr, String AppSKey, String NwkSKey);
   * Paste the example code from the TTN console here:
   */
  //const char *devAddr = "02017201";
  //const char *nwkSKey = "AE17E567AECC8787F749A62F5541D522";
  //const char *appSKey = "7C38B97AE08F10E7FB8213BB9281FF99";

  //join_result = myLora.initABP(devAddr, appSKey, nwkSKey);

  /*
   * OTAA: initOTAA(String AppEUI, String AppKey);
   * If you are using OTAA, paste the example code from the TTN console here:
   */
  const char *appEui = "0000000000000000";
  const char *appKey = "7C38B97AE08F10E7FB8213BB9281FF99";

  join_result = myLora.initOTAA(appEui, appKey);


  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");
}

String getLoraWAN_Downlink()
{
  String downlink;
  downlink = myLora.getRx();
  return downlink;
}
