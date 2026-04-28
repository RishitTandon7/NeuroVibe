#define EEG_PIN 15

void setup() {
  Serial.begin(115200);
  pinMode(EEG_PIN, INPUT);
}

void loop() {
  int val = analogRead(EEG_PIN);
  Serial.println(val);
  delay(4);
}
