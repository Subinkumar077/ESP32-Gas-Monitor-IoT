// 1. PASTE YOUR BLYNK CONFIGURATION HERE (From Step 1)
#define BLYNK_TEMPLATE_ID "TMPLxxxxxx"
#define BLYNK_TEMPLATE_NAME "Real Time Gas Detection System"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_TOKEN"

#define BLYNK_PRINT Serial // Enable debug prints

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "secrets.h"

// --- WI-FI CREDENTIALS ---
// secrets.h
#define SECRET_SSID "YOUR_WIFI_NAME"
#define SECRET_PASS "YOUR_WIFI_PASSWORD"
#define SECRET_BLYNK_AUTH "YOUR_BLYNK_TOKEN"

// --- PIN DEFINITIONS ---
#define GAS_SENSOR_PIN 34 
#define BUZZER_PIN 23     

// --- SETTINGS ---
#define GAS_THRESHOLD 1500 

// Initialize OLED 
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

// Timer for sending data (Don't flood Blynk with data!)
BlynkTimer timer;

void sendSensorData() {
  int analogValue = analogRead(GAS_SENSOR_PIN);
  int percentage = map(analogValue, 0, 4095, 0, 100);

  // Send data to Blynk App (Virtual Pin V0)
  Blynk.virtualWrite(V0, analogValue); 

  // Logic Check for Local Alarm
  if (analogValue > GAS_THRESHOLD) {
    tone(BUZZER_PIN, 1000); 
    // Optional: Send push notification to phone
    // Blynk.logEvent("gas_alert", "Gas Leak Detected!"); 
  } else {
    noTone(BUZZER_PIN); 
  }

  // Update OLED (Local Display)
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Blynk Connected");
  
  display.setCursor(0, 20);
  display.print("Gas Level: ");
  display.print(analogValue);
  
  if (analogValue > GAS_THRESHOLD) {
    display.setCursor(0, 45);
    display.setTextSize(2);
    display.print("DANGER!");
  } else {
    display.setCursor(0, 45);
    display.print("Status: Safe");
  }
  display.display();
}

void setup() {
  Serial.begin(115200);

  pinMode(GAS_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Init OLED
  display.begin(0x3C, true); 
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,10);
  display.print("Connecting WiFi...");
  display.display();

  // Connect to Blynk
  Blynk.begin(auth, ssid, pass);

  // Setup a function to be called every 1 second (1000L)
  timer.setInterval(1000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}