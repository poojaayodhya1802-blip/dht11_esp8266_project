#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include "DHT.h"

// ====== WiFi Credentials ======
const char* ssid = "Galaxy";       // Your WiFi SSID
const char* password = "12345678"; // Your WiFi Password

// ====== ThingSpeak Configuration ======
WiFiClient client;
unsigned long myChannelNumber = 3090113;          // Your ThingSpeak Channel Number
const char* myWriteAPIKey = "GYCX4ZOUP5AZWOS5";   // Your Write API Key

// ====== DHT Sensor Configuration ======
#define DHTPIN D4          // DHT11 data pin connected to GPIO D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  delay(100);

  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Read DHT11 data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Default °C

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  // Print to Serial
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send to ThingSpeak
  ThingSpeak.setField(1, temperature); // Field 1 → Temperature
  ThingSpeak.setField(2, humidity);    // Field 2 → Humidity

  int statusCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (statusCode == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error sending data. HTTP code: ");
    Serial.println(statusCode);
  }

  delay(20000); // Wait 20 seconds (ThingSpeak limit is 15 seconds)
}