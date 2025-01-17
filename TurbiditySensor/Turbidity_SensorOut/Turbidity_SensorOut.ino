void setup() {
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(A0);

  Serial.print("Sensor Output: ");
  Serial.println(sensorValue);
  delay(1000);
}