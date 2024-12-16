**FactoryHealthGuard/EMU/EMU.ino**

1. Remember to add the library for the temperature and humidity sensor, use the "DHT11 by Dhruba Saha" in Arduino IDE.
2. connection for DHT11 sensor, left to right: pin 1 to digital 6 (6), pin 2 to VCC, pin 3 to GND.
3. Connection for photoresistor, left to right: pin 1 to analog 2 (A2), pin 2 to VCC, pin 3 to GND.

**FactoryHealthGuard/PDI/PDI.ino**

1. Install the RN2483 Library: https://github.com/jpmeijers/RN2483-Arduino-Library in your Arduino IDE.
2. Pro Mini pin 11 <------> RX on RN2483
   
Pro Mini pin 10 <------> TX on RN2483

Pro Mini Vcc <------> 3.3V on RN2483

Pro Mini GND <------> GND on RN2483

Pro Mini pin 12 <------> RST on RN2483

4. redPin  3  <------> D3 on Pro Mini

greenPin  4  <------> D4 on Pro Mini

bluePin  5  <------> D5 on Pro Mini

noisePin  8  <------> D8 for noise


