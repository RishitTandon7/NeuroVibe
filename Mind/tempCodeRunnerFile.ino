void setup() {
  Serial.begin(115200);
  pinMode(34, INPUT);
}

void loop() {
  int val = analogRead(34);
  Serial.println(val);
  delay(100);  // Send data every 100ms (10Hz)
}
