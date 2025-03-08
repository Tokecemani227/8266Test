#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

#define STASSID ""
#define STAPSK ""
#define SWITCH_PIN D1  // Pin untuk switch

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiClient client;

const int numberList[] = {10, 25, 37, 49, 58, 63, 77, 82, 91}; // List angka
const int listSize = sizeof(numberList) / sizeof(numberList[0]);

unsigned long myChannelNumber = ;
const char *myWriteAPIKey = "";

bool lastSwitchState = HIGH;

void setup() {
    Serial.begin(9600);
    
    pinMode(SWITCH_PIN, INPUT_PULLUP); // Menggunakan internal pull-up
    randomSeed(analogRead(A0)); // Seed untuk random

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected");
    ThingSpeak.begin(client); // Inisialisasi ThingSpeak
}

void loop() {
    bool currentSwitchState = digitalRead(SWITCH_PIN);
    int randomIndex = -1; // Inisialisasi dengan nilai default untuk mencegah akses memori yang salah
    
    if (currentSwitchState == LOW && lastSwitchState == HIGH) {
        // Jika switch ditekan (transisi HIGH -> LOW)
        randomIndex = random(0, listSize);
        Serial.println(numberList[randomIndex]);
    }

    // Pastikan nilai randomIndex valid sebelum dikirim ke ThingSpeak
    if (randomIndex != -1 && WiFi.status() == WL_CONNECTED) {
        int response = ThingSpeak.writeField(myChannelNumber, 1, numberList[randomIndex], myWriteAPIKey);
        if (response == 200) {
            Serial.println("Channel update successful.");
        } else {
            Serial.println("Problem updating channel. HTTP error code: " + String(response));
        }
    } else if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping ThingSpeak update.");
    }

    lastSwitchState = currentSwitchState;
    delay(50); // Debounce sederhana
}
