// minddrive_thought_control.ino
// Continuous Thought-Based Car Control using ESP32 and BioAmp EXG Pill
// Reads EEG from BioAmp OUT on D15, processes onboard with moving average and trend detection,
// controls TB6612FNG motor driver for car movement.
// No Wi-Fi, no external processing - entirely onboard.

// Hardware Connections:
// - BioAmp OUT -> ESP32 D15 (analog input)
// - TB6612FNG:
//   AIN1 -> GPIO26
//   AIN2 -> GPIO25
//   BIN1 -> GPIO33
//   BIN2 -> GPIO32
//   STBY -> 3.3V (always enabled)
// - Motors powered by 7.4V battery
// - ESP32 powered via USB

// Pin Definitions
#define EEG_PIN 15  // BioAmp EXG Pill output
#define AIN1 26     // Motor A input 1
#define AIN2 25     // Motor A input 2
#define BIN1 33     // Motor B input 1
#define BIN2 32     // Motor B input 2
#define STBY 3.3    // Standby pin (connect to 3.3V)

// Processing Parameters
#define BUFFER_SIZE 10  // Moving average window size
#define SUSTAINED_THRESHOLD 20  // Number of loops avg >=1800 to trigger backward (adjust based on response time)

// Global Variables
int eegBuffer[BUFFER_SIZE];
int bufferIndex = 0;
long eegSum = 0;
int eegAverage = 0;
int prevAverage = 0;
int trend = 0;  // 1: rising, -1: falling, 0: stable
int sustainedCount = 0;

// Motor Control Functions
void moveForward() {
  // Motor A: Forward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  // Motor B: Forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void moveBackward() {
  // Motor A: Backward
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  // Motor B: Backward
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void turnLeft() {
  // Motor A: Forward
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  // Motor B: Backward (for left turn)
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void turnRight() {
  // Motor A: Backward (for right turn)
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  // Motor B: Forward
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void stopMotors() {
  // Stop both motors
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

void setup() {
  Serial.begin(115200);

  // Initialize EEG buffer
  for (int i = 0; i < BUFFER_SIZE; i++) {
    eegBuffer[i] = 0;
  }

  // Set motor pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // Enable motor driver (STBY high)
  digitalWrite(STBY, HIGH);

  // Stop motors initially
  stopMotors();

  Serial.println("MindDrive: Thought-Based Car Control Ready");
  Serial.println("EEG Pin: D15, Sampling ~250Hz");
  Serial.println("Debug: EEG, Average, Trend, Action");
}

void loop() {
  // Read EEG value
  int eegVal = analogRead(EEG_PIN);

  // Update moving average buffer
  eegSum -= eegBuffer[bufferIndex];
  eegBuffer[bufferIndex] = eegVal;
  eegSum += eegVal;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  eegAverage = eegSum / BUFFER_SIZE;

  // Compute trend (compare to previous average)
  if (eegAverage > prevAverage) {
    trend = 1;  // Rising
  } else if (eegAverage < prevAverage) {
    trend = -1; // Falling
  } else {
    trend = 0;  // Stable
  }
  prevAverage = eegAverage;

  // Determine action based on mapping logic
  String action = "STOP";
  if (eegAverage < 1000) {
    stopMotors();
    action = "STOP";
    sustainedCount = 0;  // Reset sustained counter
  } else if (eegAverage >= 1000 && eegAverage < 1500) {
    moveForward();
    action = "FORWARD";
    sustainedCount = 0;
  } else if (eegAverage >= 1500 && eegAverage < 1800) {
    if (trend > 0) {
      turnRight();
      action = "RIGHT";
    } else {
      turnLeft();
      action = "LEFT";
    }
    sustainedCount = 0;
  } else if (eegAverage >= 1800) {
    sustainedCount++;
    if (sustainedCount > SUSTAINED_THRESHOLD) {
      moveBackward();
      action = "BACKWARD";
    } else {
      moveForward();  // Continue forward until sustained
      action = "FORWARD (sustaining)";
    }
  }

  // Print debug values
  Serial.print("EEG: ");
  Serial.print(eegVal);
  Serial.print(", Avg: ");
  Serial.print(eegAverage);
  Serial.print(", Trend: ");
  Serial.print(trend > 0 ? "Rising" : (trend < 0 ? "Falling" : "Stable"));
  Serial.print(", Action: ");
  Serial.println(action);

  delay(4);  // ~250 Hz sampling (4ms delay)
}

/*
Calibration Guide:
1. Upload the sketch and open Serial Monitor at 115200 baud.
2. Relax and focus on different thoughts while monitoring the "Avg" values.
3. Note the range of your EEG averages:
   - Baseline (relaxed): should be below 1000 for STOP.
   - Light focus/intensity: 1000-1500 for FORWARD.
   - Moderate intensity with increasing: 1500-1800 rising -> RIGHT.
   - Moderate intensity with decreasing: 1500-1800 falling -> LEFT.
   - High intensity sustained: >=1800 for BACKWARD after threshold.
4. If averages are consistently outside these ranges, adjust thresholds in code:
   - Increase/decrease the numbers (e.g., 1000 to 1200) based on your readings.
   - Adjust BUFFER_SIZE for smoother average (larger = more stable but slower response).
   - Adjust SUSTAINED_THRESHOLD for backward trigger sensitivity (higher = longer hold needed).
5. Test with car on a safe surface, start with low battery voltage to avoid fast movement.
6. Ensure BioAmp is properly placed and calibrated per its datasheet.
*/
