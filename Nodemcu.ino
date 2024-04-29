#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char* ssid = "Shiva@766";
const char* password = "12345678";
const char* adminUsername = "admin";
const char* adminPassword = "admin123";

const int sensorPin = A0;

int baseline = 0;
int threshold = 200;

AsyncWebServer server(80);

String message = "";

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi network...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!request->authenticate(adminUsername, adminPassword)) {
      return request->requestAuthentication();
    }

    String webpage = "<!DOCTYPE html><html><head><title>Food Spoilage Detector</title>";
    webpage += "<style>";
    webpage += "body { font-family: Arial, sans-serif; background-color: #f5f5f5; }";
    webpage += ".container { max-width: 600px; margin: 0 auto; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); animation: fadeIn 1s ease-out; }";
    webpage += "@keyframes fadeIn { from { opacity: 0; transform: translateY(-20px); } to { opacity: 1; transform: translateY(0); } }";
    webpage += "h1, h2 { text-align: center; color: #333; margin-top: 0; animation: slideInDown 1s ease-out; }";
    webpage += "p { text-align: center; animation: slideInUp 1s ease-out; }";
    webpage += ".footer { text-align: center; margin-top: 20px; font-size: 12px; color: #666; }";
    webpage += "@keyframes slideInDown { from { opacity: 0; transform: translateY(-100%); } to { opacity: 1; transform: translateY(0); } }";
    webpage += "@keyframes slideInUp { from { opacity: 0; transform: translateY(20px); } to { opacity: 1; transform: translateY(0); } }";
    webpage += "</style>";
    webpage += "</head><body>";
    webpage += "<div class='container'>";
    webpage += "<h1>Food Spoilage Detector</h1>";
    
    int sensorValue = analogRead(sensorPin);
    int difference = abs(sensorValue - baseline);
    
    if (difference > threshold) {
      webpage += "<p style='color:red;'>Food is spoiled. Please discard.</p>";
    } else {
      webpage += "<p style='color:green;'>Food is fresh. You can eat.</p>";
    }
    
    webpage += "<h2>Sensor Values</h2>";
    webpage += "<p>Current Sensor Value: <span id='currentSensorValue'>" + String(sensorValue) + "</span></p>";
    webpage += "<p>Baseline Value: <span id='baselineValue'>" + String(baseline) + "</span></p>";
    webpage += "<p>Threshold Value: " + String(threshold) + "</p>";
    
    webpage += "<p>" + message + "</p>";
    webpage += "<script>";
    webpage += "setInterval(function() {";
    webpage += "fetch('/sensorData')";
    webpage += ".then(response => response.json())";
    webpage += ".then(data => {";
    webpage += "document.getElementById('currentSensorValue').innerText = data.currentSensorValue;";
    webpage += "document.getElementById('baselineValue').innerText = data.baselineValue;";
    webpage += "});";
    webpage += "}, 1000);";
    webpage += "</script>";
    webpage += "</div>";
    webpage += "<div class='footer'>&copy; Batch-21. All rights reserved.</div>";
    webpage += "</body></html>";
    request->send(200, "text/html", webpage);
  });

  server.on("/sensorData", HTTP_GET, [](AsyncWebServerRequest *request) {
    int sensorValue = analogRead(sensorPin);
    String responseData = "{\"currentSensorValue\": " + String(sensorValue) + ", \"baselineValue\": " + String(baseline) + "}";
    request->send(200, "application/json", responseData);
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  if (millis() < 60000) {
    baseline = baseline * 0.95 + sensorValue * 0.05;
  }

  delay(1000);
}
