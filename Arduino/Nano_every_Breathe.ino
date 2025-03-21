// === Pin Definitions ===
const int buttonPin = 2;      // Button
const int ledPin = 5;         // LED
const int sensorPin = A0;     // Pressure sensor

// === Constants for Pressure Calculation ===
const float Vcc = 5.0;
const float zeroPressureVoltage = 2.5;
const float sensitivity = 0.6;

// === Smoothing Buffer ===
const int smoothingWindow = 3;
float buffer[smoothingWindow];
int bufIndex = 0;

// === State Variables ===
bool isOn = false;
bool lastButtonState = HIGH;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  Serial.begin(9600);
  delay(300);
  Serial.println("=== ✅ Debug Mode Started ===");
  Serial.println("Press the button to toggle sensor reading and LED.");

  for (int i = 0; i < smoothingWindow; i++) buffer[i] = 0;
}

void loop() {
  // === Button press to toggle ON/OFF ===
  int reading = digitalRead(buttonPin);
  if (reading == LOW && lastButtonState == HIGH) {
    isOn = !isOn;
    digitalWrite(ledPin, isOn ? HIGH : LOW);
    Serial.print("[BUTTON] State changed to: ");
    Serial.println(isOn ? "✅ ON" : "⛔ OFF");
    delay(300);  // Debounce
  }
  lastButtonState = reading;

  // === If active, read sensor ===
  if (isOn) {
    int raw = analogRead(sensorPin);
    float voltage = raw * Vcc / 1023.0;

    // === Smoothing ===
    buffer[bufIndex] = voltage;
    bufIndex = (bufIndex + 1) % smoothingWindow;
    float sum = 0;
    for (int i = 0; i < smoothingWindow; i++) sum += buffer[i];
    float smoothed = sum / smoothingWindow;

    // === Output: ONLY smoothed voltage for Python to use ===
    Serial.println(smoothed, 3);  // Only this line is sent to Python
  } else {
    Serial.println("[STATUS] Waiting for activation...");
  }

  delay(25);  // ~40Hz 输出
}
