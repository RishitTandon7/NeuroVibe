// ESP32 LED Control via Serial Commandse:\#Builds\Brain Automtion\Mind\esp32_led_control.ino
// Receives "LED_ON" or "LED_OFF" from Python script and controls LED

#define LED_PIN 26  // Change to your LED pin (e.g., 2 for ESP32 built-in)

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Start with LED off
  Serial.println("ESP32 LED Control Ready");
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // Remove any whitespace

    if (command == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON");
    } else if (command == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF");
    } else {
      Serial.println("Unknown command: " + command);
    }
  }
}
