int tank2_lvl = 13;
int t2bpump = 4, t2apump = 6;

unsigned long t2bpumpST = 0;
unsigned long t2apumpST = 0;

bool t2bopen = false;
bool t2aopen = false;

void setup() {
  pinMode(t2bpump, OUTPUT);
  pinMode(t2apump, OUTPUT);
  pinMode(tank2_lvl, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available() == 0){
    check_tank2();

    String response = Serial.readStringUntil('\n');
    if (response) {
      response.trim();
      int separatorIndex = response.indexOf(":");
      if (separatorIndex != -1) {
        String label = response.substring(0, separatorIndex + 1); 
        String value = response.substring(separatorIndex + 1);

        // Remove extra whitespace
        label.trim(); 
        value.trim();
        label.toLowerCase();
        value.toUpperCase();

        if (value == "CHECK WATER QUALITY"){
          check_water_quality();
        }
      }
    }

    update_pump(t2bpump, &t2bpumpST, &t2bopen, 10000);
    update_pump(t2apump, &t2apumpST, &t2aopen, 5000);
  }
}

void check_water_quality(){
  int sensorValue = analogRead(A0);
  int turbidity = map(sensorValue, 0, 777, 100, 0);

  if (turbidity > 30){
    // TODO: Send response for the admin to save the turbidity value
    Serial.print("Water Quality Status: Failed! | Turbidity: ");
    Serial.println(turbidity);
    open_pump(t2bpump, &t2bpumpST, &t2bopen);
  } else {
    Serial.print("Water Quality Status: Passed! | Turbidity: ");
    Serial.println(turbidity);
    open_pump(t2apump, &t2apumpST, &t2aopen);
  }
}

void check_tank2(){
  int tank2_full = digitalRead(tank2_lvl);
  if (tank2_full){
    check_water_quality();
  }
}

// pump open and closing
void open_pump(int pumpPin, unsigned long *startTime, bool *state){
  digitalWrite(pumpPin, HIGH);
  *startTime = millis();
  *state = true;
}

void update_pump(int pumpPin, unsigned long *startTime, bool *state, int duration){
  if (*state && millis() - *startTime >= duration){
    digitalWrite(pumpPin, LOW);
    *state = false;
  }
}
