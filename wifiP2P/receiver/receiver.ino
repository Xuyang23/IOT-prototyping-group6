#include <ESP8266WiFi.h>
#include <espnow.h>

#define NUM_STRINGS 4
#define STRING_LENGTH 32

typedef struct struct_message {
    char stringArray[NUM_STRINGS][STRING_LENGTH]; // Array of strings
} struct_message;
struct_message myData;

// Executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);

    // Print each string from the stringArray
    for (int i = 0; i < NUM_STRINGS; i++) {
        Serial.print("String Array [");
        Serial.print(i);
        Serial.print("]: ");
        Serial.println(myData.stringArray[i]);
    }
    Serial.println();
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
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
  }
}

void loop() {
  
}
