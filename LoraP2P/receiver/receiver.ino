#include <SoftwareSerial.h>

SoftwareSerial LoraSerial(10, 11); // RX, TX pins for LoRa communication

String str;  // Variable to store incoming serial data

// Function declarations
void led_on();
void led_off();
void lora_autobaud();
void send_lora_command(String command);
float hexToFloat(String hexString);

// Function to convert hexadecimal string to float
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

void setup() {
  // Initialize LED
  pinMode(LED_BUILTIN, OUTPUT);
  led_off();  // Turn off LED initially

  Serial.begin(57600);  // Initialize Serial communication with PC
  LoraSerial.begin(9600);  // Initialize LoRa communication
  LoraSerial.setTimeout(1000);

  // Autobauding for LoRa Module
  lora_autobaud(); 

  // Initialize LoRa settings
  led_on(); // Turn on LED to indicate initialization
  delay(1000);
  led_off(); // Turn off LED after initialization

  init_lora();  // Initialize LoRa configuration

  Serial.println("LoRa initialization complete. Ready to receive data...");
  LoraSerial.println("radio rx 0");  // Command to start receiving LoRa data
}

void loop() {
  // Read incoming data from LoRa module
  if (LoraSerial.available()) {
    str = LoraSerial.readStringUntil('\n');  // Read the incoming message until newline
    
    // If we received a valid message
    if (str.length() > 0 && str.indexOf("radio_rx") == 0) {
      Serial.println("Received data: ");
      Serial.println(str);  // Print the received string

      // Extract the part after "radio_rx "
      String hexData = str.substring(str.indexOf("radio_rx") + 10);
      Serial.println(hexData); 

      // Convert hex data to float array
      float floatData[10]; // Array to store the received floating-point numbers
      int size;

      // Convert hex string to float array
      hexToFloatArray(hexData, floatData, size);

      // Print the converted float array
      Serial.println("Converted float array:");
      for (int i = 0; i < size; i++) {
        Serial.print(floatData[i]);
        Serial.print(" ");
      }
      Serial.println();
    } else {
      Serial.println("No data received");
    }
  }
  
  delay(1000);  // Delay before checking again
  send_lora_command("radio rx 0");
}

// Function to initialize LoRa module with specific settings
void init_lora() {
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

// Function to turn on the onboard LED
void led_on() {
  digitalWrite(LED_BUILTIN, HIGH);
}

// Function to turn off the onboard LED
void led_off() {
  digitalWrite(LED_BUILTIN, LOW);
}

