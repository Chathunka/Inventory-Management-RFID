//this works
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ssid";
const char* password = "password";
const char* mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char* mqttUser = "mqttuser";
const char* mqttPassword = "mqttuserpwd";

WiFiClient espClient;
PubSubClient client(espClient);

#define RST_PIN         22          // Configurable, see typical pin layout above
#define SS_PIN          5         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

uint8_t knownUids[10][4]; // Store up to 10 UIDs
int tagCount = 0;

byte currentUid[4];  // declare currentUid as byte array with 4 elements
byte previousUid[4]; // declare previousUid as byte array with 4 elements
int currentCount = 0; // declare currentCount as an integer and initialize it to 0
int totalCount = 0; // declare totalCount as an integer and initialize it to 0

void setup() {
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);        // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init MFRC522
  delay(4);               // Optional delay. Some board do need more time after init to be ready, see Readme
  mfrc522.PCD_DumpVersionToSerial();    // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  // Check for new cards
  if (mfrc522.PICC_IsNewCardPresent()) {
    // Select the first detected card
    if (mfrc522.PICC_ReadCardSerial()) {
      // Store the current UID
      memcpy(currentUid, mfrc522.uid.uidByte, 4);
     

      // Check if the current UID is the same as the previous UID
      if (memcmp(currentUid, previousUid, 4) == 0) {
        delay(3000);
        Serial.println("duplicate");
        currentCount++; // Increment the count for the current UID
        totalCount++; // Increment the total count
      } else {
        // The current UID is different from the previous UID, so print the counts for the previous UID and store the current UID as the new previous UID
        Serial.print("Tag UID: ");
        for (byte i = 0; i < mfrc522.uid.size; i++) {
          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(mfrc522.uid.uidByte[i], HEX);
        }
        Serial.println();
        Serial.print("Tag count: ");
        Serial.println(currentCount);

        // Publish the counts to ThingsBoard
        if (!client.connected()) {
          reconnect();
        }
        sendData();

        // Store the current UID as the new previous UID and reset the count for the current UID
        memcpy(previousUid, currentUid, 4);
        currentCount = 1;
      }

      if (totalCount % 10 == 0) {
        // Print the total count every 10 tags
        Serial.print("Total tag count: ");
        Serial.println(totalCount);
      }
    }
  }
}




void sendData() {
  char data[50];
  sprintf(data, "{\"Tag count\": %d, \"Tag UID\": \"%02X %02X %02X %02X\"}", tagCount,
    mfrc522.uid.uidByte[0], mfrc522.uid.uidByte[1], mfrc522.uid.uidByte[2], mfrc522.uid.uidByte[3]);
  client.publish("v1/devices/me/telemetry", data);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}