int pump = 5;

void setup() {
  pinMode(pump, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.trim();

    if (input.equalsIgnoreCase("on")) {
      digitalWrite(pump, HIGH);
      Serial.println("Pump is now ON");
    } else if (input.equalsIgnoreCase("off")) {
      digitalWrite(pump, LOW);
      Serial.println("Pump is OFF");
    }
  }
}
