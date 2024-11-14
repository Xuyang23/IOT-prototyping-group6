#include <SoftwareSerial.h>

SoftwareSerial LoraSerial(10, 11); // RX, TX pins for LoRa communication

String str; // String variable to store incoming serial data
String hexToAscii(String hex); // Function to convert hex string to ASCII
void led_on(); // Function to turn on the LED
void led_off(); // Function to turn off the LED
void lora_init(); // Function to initialize LoRa with specific configurations
void lora_autobaud(); // Function to perform LoRa autobauding
int wait_for_ok(); // Function to wait for "ok" response from LoRa
void toggle_led(); // Function to toggle LED on and off
void send_lora_command(String command); // Function to send LoRa command and print the response

void setup() {
  // Set up LED pin
  pinMode(LED_BUILTIN, OUTPUT);  
  led_off(); // Ensure LED is initially off
  
  // Initialize serial communication with PC
  Serial.begin(57600);
  
  // Initialize LoRa communication
  LoraSerial.begin(9600);
  LoraSerial.setTimeout(1000);
  
  // Initialize LoRa with the required settings
  lora_init(); // Initialize LoRa module
  
  led_on();  // Turn on LED to indicate initialization
  delay(1000);  // Wait for 1 second
  led_off();  // Turn off LED after initialization

  // Print out the initial status of LoRa module and set configuration
  Serial.println("Initing LoRa");
  send_lora_command("sys get ver"); // Get LoRa firmware version
  send_lora_command("mac pause"); // Pause the LoRaWAN MAC layer
  send_lora_command("radio set mod lora"); // Set modulation to LoRa
  send_lora_command("radio set freq 869100000"); // Set frequency (869 MHz)
  send_lora_command("radio set pwr 14"); // Set transmission power to 14 dBm
  send_lora_command("radio set sf sf7"); // Set spreading factor to 7
  send_lora_command("radio set afcbw 41.7"); // Set frequency correction bandwidth
  send_lora_command("radio set rxbw 20.8"); // Set receiver bandwidth
  send_lora_command("radio set prlen 8"); // Set preamble length
  send_lora_command("radio set crc on"); // Enable CRC
  send_lora_command("radio set iqi off"); // Disable IQ inversion
  send_lora_command("radio set cr 4/5"); // Set coding rate to 4/5
  send_lora_command("radio set wdt 60000"); // Set watchdog timer (disable for continuous reception)
  send_lora_command("radio set sync 12"); // Set sync word
  send_lora_command("radio set bw 125"); // Set bandwidth to 125 kHz
}

void loop() {
  // Continuously check for incoming messages
  Serial.println("waiting for a message");
  LoraSerial.println("radio rx 0");  // Start receiving
  str = LoraSerial.readStringUntil('\n');  // Read the incoming message
  Serial.println(str);  // Print the ok
  delay(20);  // Delay for stability
  
  if (str.length() > 0 && str.indexOf("ok") == 0) {  // Check if the radio has entered receive mode
    str = String("");  
    while (str == "") {
      str = LoraSerial.readStringUntil('\n');  // Read until " "
      Serial.println(str);  // Print raw received hex data
    }
    if (str.indexOf("radio_rx") == 0) {  // Check if data is received
      toggle_led();  // Toggle the LED to indicate data received
      String receivedData = str.substring(str.indexOf("radio_rx") + 10);  // +9 to skip "radio_rx "
      Serial.println(receivedData);  // Print raw received hex data
    } else {
      Serial.println("Received nothing");  // If no data received
    }
  } else {
    Serial.println("radio not going into receive mode");  // If radio did not enter receive mode
    delay(1000);  // Wait before retrying
  }
}


// Function to initialize LoRa module with specific settings
void lora_init() {
  lora_autobaud();  // Ensure LoRa module is autobauded
  led_on();  // Indicate initialization by turning LED on
  delay(1000);  // Wait for 1 second
  led_off();  // Turn off the LED
  
  Serial.println("Initing LoRa");
  send_lora_command("sys get ver");
  send_lora_command("mac pause");
  send_lora_command("radio set mod lora");
  send_lora_command("radio set freq 869100000");
  send_lora_command("radio set pwr 14");
  send_lora_command("radio set sf sf7");
  send_lora_command("radio set afcbw 41.7");
  send_lora_command("radio set rxbw 20.8");
  send_lora_command("radio set prlen 8");
  send_lora_command("radio set crc on");
  send_lora_command("radio set iqi off");
  send_lora_command("radio set cr 4/5");
  send_lora_command("radio set wdt 60000");
  send_lora_command("radio set sync 12");
  send_lora_command("radio set bw 125");
}

// Function to send LoRa command and print response
void send_lora_command(String command) {
  Serial.println(command);  // Print the command to Serial Monitor
  LoraSerial.println(command);  // Send command to LoRa module
  str = LoraSerial.readStringUntil('\n');  // Read the response from LoRa module
  while (str.length() == 0) {  // Retry until we get a valid response
    str = LoraSerial.readStringUntil('\n');
    delay(100);  // Avoid busy-wait
  }
  Serial.println(str);  // Print the response
}

// Function for LoRa auto-bauding
void lora_autobaud() {
  String response = "";
  while (response == "") {
    delay(1000);
    LoraSerial.write((byte)0x00);  // Send a byte
    LoraSerial.write(0x55);  // Send another byte
    LoraSerial.println();  // Send a newline
    LoraSerial.println("sys get ver");  // Ask for firmware version
    response = LoraSerial.readStringUntil('\n');  // Wait for a response
  }
}

// Function to check for "ok" response
int wait_for_ok() {
  str = LoraSerial.readStringUntil('\n');
  if (str.indexOf("ok") == 0) {  // Check if "ok" is received
    return 1;
  }
  else return 0;
}

// Function to toggle the onboard LED (used to indicate activity)
void toggle_led() {
  digitalWrite(LED_BUILTIN, 1);  // Turn on LED
  delay(100);  // Wait for 100ms
  digitalWrite(LED_BUILTIN, 0);  // Turn off LED
}

// Function to turn on the onboard LED
void led_on() {
  digitalWrite(LED_BUILTIN, 1);  // Set the LED pin high
}

// Function to turn off the onboard LED
void led_off() {
  digitalWrite(LED_BUILTIN, 0);  // Set the LED pin low
}

// Function to convert hex string to ASCII string
String hexToAscii(String hex) {
  if (hex.length() % 2 != 0) {
    hex = "0" + hex;  // Prepend '0' if the string length is odd
  }
  String asciiString = "";
  for (size_t i = 0; i < hex.length(); i += 2) {
    String byteString = hex.substring(i, i + 2);  // Get two characters from hex string
    char byteValue = (char)strtol(byteString.c_str(), NULL, 16);  // Convert hex to byte
    asciiString += byteValue;  // Append byte as character to the ASCII string
  }
  return asciiString;  // Return the ASCII string
}
