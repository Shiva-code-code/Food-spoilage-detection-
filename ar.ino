#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int sensorPin = A0;
const int buzzerPin = 10;

int baseline = 0;
int threshold = 200;

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display

void setup() {
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  // Initialize the LCD
  lcd.begin();  // Initialize the LCD without specifying columns and rows
  lcd.backlight();  // Turn on the backlight
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // Adjust baseline value in the first 100 readings (optional)
  if (millis() < 60000) {
    baseline = baseline * 0.95 + sensorValue * 0.05;
  }

  int difference = abs(sensorValue - baseline);

  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
  Serial.print("Difference: ");
  Serial.println(difference);

  // Display messages on LCD based on the difference value
  if (sensorValue > 100) {
    digitalWrite(buzzerPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Food is spoiled.");
    lcd.setCursor(0, 1);
    lcd.print("Please discard.");
    Serial.println("Food spoilage smell detected!");
  } else {
    digitalWrite(buzzerPin, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Food is fresh.");
    lcd.setCursor(0, 1);
    lcd.print("You can eat.");
  }

  delay(1000);
}
