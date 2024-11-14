#include <DHT11.h>
// Remember to add the library for the temperature and humidity sensor, use the "DHT11 by Dhruba Saha" in Arduino IDE.
// connection for DHT11 sensor, left to right: pin 1 to digital 6 (6), pin 2 to VCC, pin 3 to GND.
// Connection for photoresistor, left to right: pin 1 to analog 2 (A2), pin 2 to VCC, pin 3 to GND.  

DHT11 dht11(6);

void setup() {
  Serial.begin(9600);
}

void loop() {
  int temperature = 0;
  int humidity = 0;
  int photores = 0;

  // Read temperature and humidity
  int result = dht11.readTemperatureHumidity(temperature, humidity);

  if (result == 0) {
    // Convert each part individually into strings
    String tempString = "Temperature: " + String(temperature) + " °C";
    String humidityString = "Humidity: " + String(humidity) + " %";
    
    // Read photoresistor value and convert to string
    photores = analogRead(2);
    String photoresString = "Photoresistor: " + String(photores);

    // Print each string individually
    Serial.println(tempString);
    Serial.println(humidityString);
    Serial.println(photoresString);
  } else {
    Serial.println(DHT11::getErrorString(result));
  }

  delay(1000);
}
