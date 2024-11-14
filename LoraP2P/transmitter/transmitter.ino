#include <SoftwareSerial.h>

SoftwareSerial LoraSerial(10, 11); // RX, TX

String str; // Declaration of a String variable to store incoming serial data
int packageID = 0; // Initialization of a variable to store package ID

// Function declarations
void led_on(); // Function to turn on the LED
void led_off(); // Function to turn off the LED
void lora_autobaud(); // Function to perform LoRa autobauding
int wait_for_ok(); // Function to wait for "ok" response
String asciiToHex(String asciiString); 
void init_lora(); // Function to initialize LoRa settings
void send_lora_command(String command); // Function to send LoRa commands and print response

void setup() {
  //output LED pin
  pinMode(LED_BUILTIN, OUTPUT);  // D7 on ESP8266
  led_off(); // Ensure LED is initially turned off
  
  // Open serial communications and wait for port to open:
  Serial.begin(57600);  // Serial communication to PC
  
  LoraSerial.begin(9600);  // Serial communication to RN2483
  LoraSerial.setTimeout(1000);
  lora_autobaud(); // Perform LoRa autobauding
  
  led_on(); // Turn on LED briefly to indicate initialization
  delay(1000);
  led_off(); // Turn off LED after initialization
  
  init_lora();  // Initialize LoRa settings
  Serial.println("starting loop");
}

void loop() {
  led_on();
  Serial.print("packageID = ");
  Serial.println(packageID);

  // Prepare the message to send
  char test_str[16] = "Hello";
  String hexMessage = asciiToHex(test_str);

  // Print the message to the Serial Monitor before sending it
  Serial.print("Sending message: ");
  Serial.println(hexMessage);  // Print the hex string of the message
  
  LoraSerial.print("radio tx ");
  LoraSerial.println(hexMessage);  // Send the message via LoRa
  // Wait for the response and print it
  str = LoraSerial.readStringUntil('\n');
  if (str.length() > 0) {
    Serial.println(str);
  } else {
    Serial.println("Timeout waiting for response");
  }

  // Check again for response (depending on your LoRa module setup)
  str = LoraSerial.readStringUntil('\n');
  if (str.length() > 0) {
    Serial.println(str);
  } else {
    Serial.println("Timeout waiting for response");
  }

  led_off();
  delay(2000);  // Wait for 2 seconds before sending the next message
  packageID = packageID + 1;
}


void init_lora() {
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
}

void send_lora_command(String command) {
  // Send LoRa command and print response
  Serial.println(command);
  LoraSerial.println(command);
  if (wait_for_ok()) { // Wait for "ok" confirmation
    Serial.println("Command executed successfully.");
  } else {
    Serial.println("Failed to execute command.");
  }
}

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
  str = LoraSerial.readStringUntil('\n');
  if (str.indexOf("ok") == 0) {
    return 1;
  } else {
    return 0;
  }
}

void led_on() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void led_off() {
  digitalWrite(LED_BUILTIN, LOW);
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
