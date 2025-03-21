// === 引脚定义 ===
const int pumpPin = 5;          // CZ321 气泵
const int inflateValvePin = 9;  // HU591 充气阀
const int deflateValvePin = 6;  // HU591 放气阀
const int ledPin = 13;          // 抽搐指示灯（板载）

// === 状态变量 ===
float lastValue = 0;
float lastDerivative = 0;

bool isContracting = false;
unsigned long contractStartTime = 0;
const unsigned long contractDuration = 150;  // 抽搐时间（毫秒）

void setup() {
  pinMode(pumpPin, OUTPUT);
  pinMode(inflateValvePin, OUTPUT);
  pinMode(deflateValvePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(9600);
  Serial.println("🌊 UNO 控制启动完成");
}

void loop() {
  // === 1. 收缩控制 ===
  if (isContracting) {
    if (millis() - contractStartTime < contractDuration) {
      // 抽搐中 → 关闭泵，强制放气，亮灯
      analogWrite(pumpPin, 0);
      analogWrite(inflateValvePin, 0);
      analogWrite(deflateValvePin, 255);
      digitalWrite(ledPin, HIGH);
      return;  // 不读取串口
    } else {
      // 恢复
      analogWrite(deflateValvePin, 0);
      digitalWrite(ledPin, LOW);
      isContracting = false;
    }
  }

  // === 2. 串口接收数据 ===
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // 清理\r等字符
    int sep = input.indexOf('|');

    if (sep > 0 && sep < input.length() - 1) {
      float value = input.substring(0, sep).toFloat();
      float derivative = input.substring(sep + 1).toFloat();
      lastValue = value;
      lastDerivative = derivative;

      // === 3. 状态判断逻辑 ===

      // ⚡ 抽搐逻辑（强烈导数变化）
      if (abs(derivative) > 300) {
        isContracting = true;
        contractStartTime = millis();
        analogWrite(pumpPin, 0);
        analogWrite(inflateValvePin, 0);
        analogWrite(deflateValvePin, 255);
        digitalWrite(ledPin, HIGH);
        return;
      }

      // === 映射 PWM ===
      // 如果你传来的值范围是 -500~+500，可以适配到 0~255
      int pwmValue = constrain(map(abs(value), 0, 500, 0, 255), 0, 255);

      // 🌬️ 正值 → 充气
      if (value > 3) {
        analogWrite(pumpPin, pwmValue);
        analogWrite(inflateValvePin, pwmValue);
        analogWrite(deflateValvePin, 0);
      }
      // 💨 负值 → 放气
      else if (value < -3) {
        analogWrite(pumpPin, 0);
        analogWrite(inflateValvePin, 0);
        analogWrite(deflateValvePin, pwmValue);
      }
      // 😐 静止
      else {
        analogWrite(pumpPin, 0);
        analogWrite(inflateValvePin, 0);
        analogWrite(deflateValvePin, 0);
      }
    }
  }
}
