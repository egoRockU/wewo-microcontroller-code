void setup() {
  Serial.begin(9600);
 
}
void loop() {
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1024.0);
  int turbidity = map(sensorValue, 0, 777, 100, 0);

 
  // float NTU = 100.0 * voltage;

  Serial.print ("Sensor Output:");
  Serial.println (sensorValue);
  Serial.print ("Sensor Volts:");
  Serial.println (voltage);
  Serial.print ("Turbidity (NTU): ");
  Serial.println(turbidity);
  Serial.println();
  delay(1000);
}