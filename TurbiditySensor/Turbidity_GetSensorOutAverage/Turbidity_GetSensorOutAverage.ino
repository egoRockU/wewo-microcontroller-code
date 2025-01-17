float average_output();

void setup() {
  Serial.begin(115200);
  Serial.println("Write 'start' to calculate average sesor output");
}

void loop() {
  if (Serial.available()){
    String input = Serial.readStringUntil('\n');
    if (input == "start"){
      average_output();
    }
  }
}

void average_output(){
  String loading[8] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};
  int load_sign;
  int cycles = 10;
  int average = 0;
  int sensorValue = analogRead(A0);

  for (int i = 0; i < cycles; i++){
    if (load_sign <= 7){
      Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
      Serial.println("Calculating average output " + loading[load_sign]);
      load_sign++;
    } else {
      load_sign = 0;
    }

    average += sensorValue;
    delay(1000); 
  }

  average = average / cycles;
  Serial.println("Average Sensor Output: " + String(average));
  Serial.println("\n");
  Serial.println("Write 'start' to calculate average sensor output");
}