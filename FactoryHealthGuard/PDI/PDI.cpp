#include "PDI.h"

float floatData[sensor_num] ={0,0,0};
float Threshold[sensor_num] = {100,100,100};
// Function to receive data from LoRa
void LOOP() {
  String str = "";
  
  if (LoraSerial.available()) {

    str = LoraSerial.readStringUntil('\n');  // Read the incoming message until newline
  
    if (str.length() > 0 && str.indexOf("radio_rx") == 0) {  // Check if message starts with "radio_rx"
      Serial.println("Received data: ");
      Serial.println(str);

      // Extract hex data after "radio_rx "
      String hexData = str.substring(str.indexOf("radio_rx") + 10);
      Serial.println(hexData); 

      // Convert hex string to float array
      int size;
      hexToFloatArray(hexData, floatData, size);
      
      // Process received data
      processReceivedData(size);
    } else {
      Serial.println("No valid data received");
      send_lora_command("radio rx 0");  // Command to start receiving LoRa data
    }
  }

  delay(1000);
}

// Function to process received data
void processReceivedData(int size) {
  Serial.println("Converted float array:");
  for (int i = 0; i < size; i++) {
    Serial.print(floatData[i]);
    Serial.print(" ");
  }
  Serial.println();

  // Check if alarm conditions are met
  if(size == 3){
    checkForAlarm(size);

    sendDataOverLoRaWAN(floatData, size);
  }
  else
  {
    Serial.println("Payload Length error:");
  }
}

// Function to check for alarm conditions
void checkForAlarm(int size) {
  
  if (floatData[0] >= Threshold[0] || floatData[1] >= Threshold[1] || floatData[2] >= Threshold[2]) {
    Serial.println("Environment is Unsafe");
    triggerAlarm();
  }
  
  else
  Serial.println("Environment is Safe");
  
}
// Function to trigger the alarm actions
void triggerAlarm() {
  redAlarm();
  noiseStream();
  yellowAlarm();
  noiseStream();
}

// 使用 LoRa 模块发送浮点数据
void sendDataOverLoRaWAN(float* floatData, int size) {
  initialize_radio();
  String hexString = floatArrayToHex(floatData, size);
  Serial.print("Hex representation of float data: ");
  Serial.println(hexString);

  int byteArraySize = hexString.length() / 2 + 1;
  uint8_t byteArray[byteArraySize];

  byteArray[0] = 0xAA; // 标志位，用于指示数据有效性
  hexStringToByteArray(hexString, byteArray + 1, byteArraySize);

  // 打印字节数组
  Serial.print("Byte Array: ");
  for (int i = 0; i < byteArraySize; ++i) {
    Serial.print("0x");
    if (byteArray[i] < 0x10) Serial.print("0");
    Serial.print(byteArray[i], HEX);
    if (i < byteArraySize - 1) Serial.print(", ");
  }
  Serial.println();
  Serial.println("Start transmitting Data!");
  TX_RETURN_TYPE result = myLora.txBytes(byteArray, byteArraySize);

  if (result == TX_SUCCESS || result == TX_WITH_RX) {
      // 等待 RX1 窗口开启，通常 RX1 延迟为 5 秒
      delay(5000); // 单位：毫秒

      // 获取下行数据
      String downlinkMessage = myLora.getRx();
      if(downlinkMessage.length()!=0)
      {
      Serial.println("Received downlink message");
      processDownlink(downlinkMessage);
      }
      else
      Serial.println("Message send successfully");
  } else {
      Serial.println("Failed to send uplink message.");
  }
  init_lora();  // Initialize LoRaP2P configuration
  Serial.println("LoRaP2P initialization complete. Ready to receive data...");
}

// 处理下行数据
void processDownlink(const String downlinkPayload) {
  float thresholds[3];
  int size = 0;
  hexToFloatArray(downlinkPayload, thresholds, size); // 自定义 hexToFloatArray 函数
  if(size == sensor_num){
  Serial.println("Converted float array:");
  for (int i = 0; i < size; ++i) {
    Threshold[i] = thresholds[i];
    Serial.print(Threshold[i]);
    Serial.print(" ");
  }
  Serial.println();
  }
  else
  Serial.println("Payload Length error");
}
