int pump = 5;
int pumpDuration = 1000;

void setup() {
  pinMode(pump, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int sensorValue = analogRead(A0);
  int turbidity = map(sensorValue, 0, 777, 100, 0);

  if (turbidity > 10){
    Serial.print("Turbidity: ");
    Serial.println(turbidity);
    digital.write(pump, HIGH);
    delay(pumpDuration);
    digital.write(pump, LOW);
  }
}
