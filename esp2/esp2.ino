/*
 * MindDrive v2 — ESP2 (Car)
 * Final Version (using PWMA=25, AIN1=26, AIN2=27, PWMB=33, BIN1=14, BIN2=12)
 * Fixes endless spin by controlling PWM enable pins directly.
 */

#include <WiFi.h>
#include <ESPmDNS.h>

// ---- Wi-Fi credentials ----
const char* ssid     = "LEGION-5-PRO";
const char* password = "12345679";

// ---- TB6612FNG pin mapping ----
#define PWMA 25
#define AIN1 26
#define AIN2 27
#define PWMB 33
#define BIN1 14
#define BIN2 12

WiFiServer server(8080);

// ---- function declarations ----
void stopCar();
void enableMotors(bool state);

void setup() {
  Serial.begin(115200);

  // --- Motor pin setup ---
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // --- Immediately stop all motors ---
  stopCar();
  delay(500);
  Serial.println("🛑 Motors stopped at startup");

  // --- Wi-Fi + mDNS ---
  connectWiFi();
  setupMDNS();

  server.begin();
  Serial.println("🚘 Car ESP Ready (mDNS: car.local)");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("🧠 Brain Connected!");
    stopCar();  // safety stop before any movement

    while (client.connected()) {
      if (client.available()) {
        String cmd = client.readStringUntil('\n');
        cmd.trim();
        if (!cmd.length()) continue;

        Serial.println("Received: " + cmd);

        if      (cmd == "CAR_FWD") moveForward();
        else if (cmd == "CAR_BACK") moveBackward();
        else if (cmd == "CAR_LEFT") turnLeft();
        else if (cmd == "CAR_RIGHT") turnRight();
        else stopCar();
      }
    }

    stopCar();  // safety stop on disconnect
    client.stop();
    Serial.println("❌ Brain Disconnected, Motors Stopped");
  }
}

// ---- Wi-Fi + mDNS ----
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi Connected!");
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
}

void setupMDNS() {
  if (!MDNS.begin("car")) {
    Serial.println("⚠️ mDNS failed to start");
  } else {
    Serial.println("mDNS responder started as car.local");
  }
}

// ---- Motor control helpers ----
void enableMotors(bool state) {
  if (state) {
    analogWrite(PWMA, 255); // full speed
    analogWrite(PWMB, 255);
  } else {
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
  }
}

void moveForward() {
  enableMotors(true);
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  Serial.println("🚗 FORWARD");
}

void moveBackward() {
  enableMotors(true);
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  Serial.println("⬅️ BACKWARD");
}

void turnLeft() {
  enableMotors(true);
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW);
  Serial.println("↪️ LEFT");
}

void turnRight() {
  enableMotors(true);
  digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH);
  Serial.println("↩️ RIGHT");
}

void stopCar() {
  enableMotors(false);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
  Serial.println("🛑 STOPPED");
}

