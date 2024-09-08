#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Define OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declare the SSD1306 display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ThingSpeak Channel Settings
const char* apiKey = "BKIJNOYDM205J2HG";  // ThingSpeak Read API Key
const char* thingSpeakURL = "http://api.thingspeak.com/channels/2326256/feeds.json?results=2";  // URL to fetch data

// WiFi credentials
const char* ssid = "server";
const char* password = "jeris6467";

// Function to initialize WiFi
void initWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Setup function
void setup() {
  // Initialize Serial communication
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 OLED initialization failed!"));
    while (1);  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize WiFi connection
  initWiFi();
}

// Function to fetch data from ThingSpeak
String fetchData() {
  HTTPClient http;
  http.begin(thingSpeakURL);
  
  int httpResponseCode = http.GET();
  String payload = "";
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();  // Get the response payload
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  
  return payload;
}

// Function to display data on the OLED
void displayData(String suhu, String kelembaban, String tekanan, String embun) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  
  display.print(F("T: "));
  display.print(suhu);
  display.write(167);  // Degree symbol
  display.println("C");

  display.print(F("H: "));
  display.println(kelembaban + " %");

  /*display.print(F("P: "));
  display.println(tekanan + "hPa");

  display.print(F("D: "));
  display.print(embun);
  display.write(167);  // Degree symbol
  display.println("C");*/

  display.display();  // Refresh the OLED screen
}

// Main loop
void loop() {
  String response = fetchData();
  
  // Parse JSON using ArduinoJson library
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Access the first set of data (most recent)
  JsonObject feeds0 = doc["feeds"][0];
  String suhu = feeds0["field1"];  // Get suhu (temperature) from field1
  String kelembaban = feeds0["field2"];  // Get kelembaban (humidity) from field2
  String tekanan = feeds0["field3"];  // Get tekanan (pressure) from field3
  String embun = feeds0["field4"];  // Get embun (dew point) from field4

  // Print data to Serial monitor (for debugging)
  Serial.print("Suhu: ");
  Serial.println(suhu);
  Serial.print("Kelembaban: ");
  Serial.println(kelembaban);
  Serial.print("Tekanan: ");
  Serial.println(tekanan);
  Serial.print("Embun: ");
  Serial.println(embun);

  // Display data on the OLED screen
  displayData(suhu, kelembaban, tekanan, embun);

  // Wait for 15 seconds before fetching new data
  delay(15000);
}
