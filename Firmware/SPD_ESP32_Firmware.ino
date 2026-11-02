#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

TFT_eSPI tft = TFT_eSPI();
WebServer server(80);

// Enter your Wi-Fi credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

void handleIncomingTrack() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "Body missing");
    return;
  }
  
  String body = server.arg("plain");
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (!error) {
    String trackName  = doc["track"].as<String>();
    String artistName = doc["artist"].as<String>();
    String albumName  = doc["album"].as<String>();

    // Clear display canvas and draw background border frame
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(5, 5, tft.width() - 10, tft.height() - 10, TFT_GREEN);

    // Track Title Text Rendering
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(15, 30);
    tft.println(trackName.substring(0, 18));

    // Artist Name Text Rendering
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(15, 80);
    tft.print("By: ");
    tft.println(artistName.substring(0, 18));

    // Album Name Text Rendering
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setCursor(15, 130);
    tft.print("Album: ");
    tft.println(albumName.substring(0, 16));

    server.send(200, "text/plain", "Received");
  } else {
    server.send(400, "text/plain", "JSON Parse Failed");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Display initial configuration setups matching pin mappings
  tft.init();
  tft.setRotation(1); // Landscape view layout orientation
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.setCursor(10, 20);
  tft.println("Connecting WiFi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Server Operational!");
  
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, 60);
  tft.print("IP: ");
  tft.println(WiFi.localIP()); // Direct your Python target POST requests to this IP

  // Listen for POST data at the base route address path
  server.on("/", HTTP_POST, handleIncomingTrack);
  server.begin();
}

void loop() {
  server.handleClient();
}
