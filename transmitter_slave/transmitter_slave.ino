#include <ESP8266WiFi.h>
#include <espnow.h>

// Reciever MAC Address goes here:
uint8_t receiverMACAddress[] = {0xC8, 0xC9, 0xA3, 0x69, 0xB7, 0xDA};

// Must match the receiver structure
#define NUM_STRINGS 4
#define STRING_LENGTH 32

// [senderID][measure1][measure2][measure3]
typedef struct struct_message {
    char stringArray[NUM_STRINGS][STRING_LENGTH]; // Array of strings
} struct_message;
struct_message myData;

unsigned long lastTime = 0;  
unsigned long timerDelay = 5000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
 
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } 
  else {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    // Register peer
    esp_now_add_peer(receiverMACAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }
}
 
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Set values to send
    strcpy(myData.stringArray[0], "D0001");
    sprintf(myData.stringArray[1], tempsensor()); // Random integer as string
    sprintf(myData.stringArray[2], smoke()); // Floating-point number as string
    strcpy(myData.stringArray[3], );

    // Send message via ESP-NOW
    esp_now_send(receiverMACAddress, (uint8_t *) &myData, sizeof(myData));
    lastTime = millis();
  }
}

void tempsensor() {

}