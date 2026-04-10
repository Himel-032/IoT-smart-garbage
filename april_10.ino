#include "HX711.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#define TRIG 5
#define ECHO 18

const char* ssid = "iPhone";
const char* password = "12345678";

// const char* serverName = "https://smart-garbage-production.up.railway.app/api/bins/data";
const char* serverName = "https://smart-garbage-1.onrender.com/api/bins/data";
const char* apiKey = "qklsdgiqKLEIEetLFS";

HX711 scale;

float calibration_factor = -420;
float units;
float ounces;

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 calibration sketch");

  scale.begin(4, 2);  // DOUT pin, CLK pin
  scale.set_scale();
  scale.tare();  // reset to zero

  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);


  // Sonar
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  Serial.println("System Ready");

   WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");

  // DNS test
  IPAddress ip;
  if (WiFi.hostByName("smart-garbage-1.onrender.com", ip)) {
    Serial.print("DNS OK: ");
    Serial.println(ip);
  } else {
    Serial.println("DNS FAILED");
  }
}

void loop() {
  scale.set_scale(calibration_factor);

  units = scale.get_units(10);
  if (units < 0) units = 0.00;

  Serial.print("Reading: ");
  Serial.print(units);
  Serial.print(" grams");
  Serial.print(" | calibration_factor: ");
  Serial.println(calibration_factor);

  // if (Serial.available()) {
  //   char temp = Serial.read();
  //   if (temp == '+' || temp == 'a') calibration_factor += 1;
  //   else if (temp == '-' || temp == 'z') calibration_factor -= 1;
  // }


  // --------- Sonar ----------
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = 0;

  if (duration != 0) {
    distance = duration * 0.0343 / 2;
  }

    // --------- Print ----------
  Serial.print("Distance: ");
  Serial.print(distance, 2);
  Serial.println(" cm");

  delay(1000);

  float weight = units;

   if (WiFi.status() == WL_CONNECTED) {

    WiFiClientSecure client;
    client.setInsecure();   // Skip certificate validation (OK for testing)

    HTTPClient https;

    if (https.begin(client, serverName)) {

      https.addHeader("Content-Type", "application/json");
      https.addHeader("Authorization", apiKey);

      String jsonData = "{";
      jsonData += "\"device_name\":\"BIN_1\",";
      jsonData += "\"weight_gm\":" + String(weight) + ",";
      jsonData += "\"distance_cm\":" + String(distance);
      jsonData += "}";

      int httpResponseCode = https.POST(jsonData);

      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String response = https.getString();
      Serial.println("Server response:");
      Serial.println(response);

      https.end();
    }
  }

  delay(10000); // Send every 30 seconds
}
