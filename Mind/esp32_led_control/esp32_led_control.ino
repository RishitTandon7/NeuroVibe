// ESP32 LED Control via Serial Commands with EEG Data Streaming
// Sends EEG data from BioAmp on pin 34 continuously
// Receives "LED_ON" or "LED_OFF" from Python script and controls LED on pin 26

#define LED_PIN 26
#define EEG_PIN 34

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(EEG_PIN, INPUT);
  digitalWrite(LED_PIN, LOW);  // Start with LED off
  Serial.println("ESP32 LED Control Ready - Streaming EEG data from pin 34");
}

void loop() {
  // 1. Read EEG value from BioAmp on pin 34
  int eegValue = analogRead(EEG_PIN);

  // 2. Stream EEG data continuously to Python
  Serial.println(eegValue); // You can also send in CSV format if needed

  // 3. Check if command is sent from Python to control LED
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON");
    }
    else if (command == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF");
    }
    else {
      Serial.println("Unknown command: " + command);
    }
  }

  delay(5); // Small delay for stability (200 Hz data rate)
}
