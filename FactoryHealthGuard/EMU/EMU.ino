#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <DHT11.h>
// Remember to add the library for the temperature and humidity sensor, use the "DHT11 by Dhruba Saha" in Arduino IDE.
// connection for DHT11 sensor, left to right: pin 1 to digital 6 (6), pin 2 to VCC, pin 3 to GND.
// Connection for photoresistor, left to right: pin 1 to analog 2 (A2), pin 2 to VCC, pin 3 to GND.  

#define SLEEP_CYCLES 1 // Sleep for 15 cycles each time, 15 * 8s = 120s
volatile int sleepCounter = 0; // Used to count the sleep cycles

SoftwareSerial LoraSerial(10, 11); // RX, TX

// DHT11 setup
DHT11 dht11(6); // DHT11 connected to digital pin 6



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
void sendUntilSuccess(int temperature, int humidity, int photores);
String floatArrayToHex(float floatArray[], int size);
void pin6WakeupISR();


void setup() {
  // Update WDT before it triggers
  wdt_enable(WDTO_8S);
  // Enable the Watchdog Timer interrupt
  WDTCSR |= 1 << WDIE;
  // Enable interrupts
  sei();
  // Set up sleep mode and enable it
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

   
   // Configure pin 6 as input mode and enable the pull-up resistor
  //pinMode(6, INPUT_PULLUP);

  // Configure pin 6 as external interrupt
  //attachInterrupt(digitalPinToInterrupt(6), pin6WakeupISR, CHANGE);


  
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
  int temperature = 0;
  int humidity = 0;
  int photores = 0;

  if (sleepCounter < SLEEP_CYCLES) {
    // Enter sleep mode
    sleep_cpu();
    return;
  }
  // Resume normal operation
  sleepCounter = 0; // Reset the counter

  // Read temperature and humidity
  int result = dht11.readTemperatureHumidity(temperature, humidity);
  photores = analogRead(2);

  if (result == 0) {
    // Convert each part individually into strings
    String tempString = "Temperature: " + String(temperature) + " °C";
    String humidityString = "Humidity: " + String(humidity) + " %";
    
    // Read photoresistor value and convert to string
    // photores = analogRead(2);
    String photoresString = "Photoresistor: " + String(photores);

    // Print each string individually
    Serial.println(tempString);
    Serial.println(humidityString);
    Serial.println(photoresString);
  } else {
    Serial.println(DHT11::getErrorString(result));
  }

  // Loop and send until successful
  sendUntilSuccess(temperature, humidity, photores);

  /////////////////////////// Insert original code for testing here
//  led_on();
//  Serial.print("packageID = ");
//  Serial.println(packageID);
//
//  // Prepare the message to send
//  // debug data
//  //float floatData[3] = {3.1415,3.1415,132.887};
//  float floatData[3] = {temperature,humidity,photores};
//  String hexString = floatArrayToHex(floatData, 3);
//  //String hexString = String(temperature) + String(humidity) + String(photores);
//
//  // Print the message to the Serial Monitor before sending it
//  Serial.print("Sending message: ");
//  Serial.println(hexString);  // Print the hex string of the message
//  
//  LoraSerial.print("radio tx ");
//  LoraSerial.println(hexString);  // Send the message via LoRa
//  // Wait for the response and print it
//  str = LoraSerial.readStringUntil('\n');
//  if (str.length() > 0) {
//    Serial.println(str);
//  } else {
//    Serial.println("Timeout waiting for response");
//  }
//
//  // Check again for response (depending on your LoRa module setup)
//  str = LoraSerial.readStringUntil('\n');
//  if (str.length() > 0) {
//    Serial.println(str);
//  } else {
//    Serial.println("Timeout waiting for response");
//  }
//
//  led_off();
//  //delay(2000);  // Wait for 2 seconds before sending the next message
//  packageID = packageID + 1;
  //////////////////////////////////////////////////////////////////////////////


  sleep_cpu();
}


void sendUntilSuccess(int temperature, int humidity, int photores) {
  led_on();
  Serial.print("packageID = ");
  Serial.println(packageID);
  
  float floatData[3] = {temperature, humidity, photores};
  String hexString = floatArrayToHex(floatData, 3);

  bool success = false;
  while (!success) {
    Serial.print("Sending message: ");
    Serial.println(hexString);

    LoraSerial.print("radio tx ");
    LoraSerial.println(hexString);

    // Read response, check if sending was successful
    str = LoraSerial.readStringUntil('\n');
    if (str.length() > 0) {
      success = true;
      Serial.println("Message sent successfully.");
    } else {
      Serial.println("Send failed, retrying...");
      delay(1000); // Wait for 1 second before retrying
    }
  }
  led_off();
  delay(2000);  // Wait for 2 seconds before sending the next message
  packageID = packageID + 1;
}




ISR (WDT_vect) {
 // Reset WDIE to 1
 WDTCSR |= 1 << WDIE;
 sleepCounter++; // Increase the counter
}

void pin6WakeupISR() {
  WDTCSR |= 1 << WDIE;
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
