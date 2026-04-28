/*
 * MindDrive v3 — ESP1 (Brain)
 * Streams EEG data from BioAmp EXG Pill (pin 34)
 * Modes:
 *   1️⃣ LED Mode — controls LED directly via Serial
 *   2️⃣ Car Mode — connects to Wi-Fi & sends EEG to Python over Serial
 */

#include <WiFi.h>
#include <ESPmDNS.h>

// ---------- User Wi-Fi credentials ----------
const char* ssid     = "LEGION-5-PRO";
const char* password = "12345679";

// ---------- Pins ----------
#define EEG_PIN 34
#define LED_PIN 26

// ---------- Variables ----------
bool carMode = false;  // Default: LED mode
bool connectedToCar = false;
WiFiClient carClient;

// ---------- Function declarations ----------
void setupWiFiAndMDNS();
void connectToCar();
void streamEEGData();

void setup() {
  Serial.begin(115200);
  pinMode(EEG_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("\n=== MindDrive Brain (Dynamic) ===");
  Serial.println("Select Mode:");
  Serial.println("1️⃣ LED Mode (Default)");
  Serial.println("2️⃣ Car Mode (Wi-Fi)");
  
  // --- Wait for Serial Monitor ---
  unsigned long startWait = millis();
  while (!Serial && millis() - startWait < 5000) delay(100);
  
  // --- Wait for user input ---
  Serial.println("⌛ Waiting 10 seconds for input (Press '2' for Car Mode)...");
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == '2') {
        carMode = true;
        break;
      }
    }
    delay(100);
  }

  if (carMode) {
    Serial.println("🚗 Car Mode Selected");
    setupWiFiAndMDNS();
  } else {
    Serial.println("💡 LED Mode Selected (Default)");
  }

  delay(500);
}

void loop() {
  // Always stream EEG data via Serial
  int eeg_val = analogRead(EEG_PIN);
  Serial.println(eeg_val);
  delay(4);  // ~250 Hz sampling rate

  // Handle Serial commands in LED mode
  if (!carMode && Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "LED_ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED turned ON");
    } else if (cmd == "LED_OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED turned OFF");
    }
  }

  // Car mode (Wi-Fi connection handled in Python)
  if (carMode && !connectedToCar && WiFi.status() == WL_CONNECTED) {
    // Keep searching for car if not connected
    static unsigned long lastRetry = 0;
    if (millis() - lastRetry > 5000) {
      connectToCar();
      lastRetry = millis();
    }
  }
}

// ---------- Wi-Fi + mDNS ----------
void setupWiFiAndMDNS() {
  Serial.println("\n🌐 Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long wifiTimeout = 20000;  // 20s timeout

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi Connected!");
    Serial.print("📶 SSID: "); Serial.println(ssid);
    Serial.print("📡 IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Wi-Fi connection failed. Check hotspot band (2.4GHz).");
  }

  if (!MDNS.begin("brain")) {
    Serial.println("⚠️ mDNS start failed!");
  } else {
    Serial.println("🌐 mDNS started as brain.local");
  }

  connectToCar();
}

// ---------- Connect to Car (Manual IP entry) ----------
void connectToCar() {
  Serial.println("\n🔍 Wi-Fi connected successfully!");
  Serial.println("Please enter Car ESP IP (e.g., 192.168.137.42):");

  String ipInput = "";
  unsigned long start = millis();

  while (ipInput.length() < 7) {
    if (Serial.available()) {
      ipInput = Serial.readStringUntil('\n');
      ipInput.trim();
    }
    if (millis() - start > 30000) {
      Serial.println("⚠️ Timeout waiting for IP input! Defaulting to 192.168.4.30");
      ipInput = "192.168.4.30";
      break;
    }
    delay(100);
  }

  IPAddress carIP;
  if (!carIP.fromString(ipInput)) {
    Serial.println("❌ Invalid IP format! Using fallback 192.168.4.30");
    carIP = IPAddress(192, 168, 4, 30);
  }

  Serial.print("Attempting to connect to Car ESP at: ");
  Serial.println(carIP);

  if (carClient.connect(carIP, 8080)) {
    connectedToCar = true;
    Serial.println("🚘 Connected to Car ESP successfully!");
  } else {
    Serial.println("❌ Failed to connect to Car ESP. Will retry...");
  }
}
