
// pins
int t1pump = 3, t2bpump = 4, t2apump = 5, rPump = 9;
int level1 = 10, level2 = 11, tank1_lvl = 12;

// pump delays
int tank2pumpDelay = 1000;

// StartTime
unsigned long t1pumpStartTime = 0;

// States
bool t1open = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(t1pump, OUTPUT);
  pinMode(t2bpump, OUTPUT);
  pinMode(t2apump, OUTPUT);
  pinMode(rPump, OUTPUT);
  pinMode(level1, INPUT);
  pinMode(level2, INPUT);
  pinMode(tank1_lvl, INPUT);
  Serial.begin(115200);
}

void loop() {
  // listen to serial connection
  while(Serial.available() == 0){

    check_tank1();

    String response = Serial.readStringUntil('\n');
    Serial.println(response);
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
        value.toLowerCase();

        if (value == "open tank 1"){
          open_t1pump();
        }
        else if (value == "check water quality"){
          check_water_quality();
        }
        else if (value == "check water level"){
          check_water_level();
        }
        else if (label == "res:" && isDigit(value[0])) {
          Serial.println(value);
          int response_val = value.toInt();
          open_rPump(response_val);
        }
      }
    }

    // update states
    update_t1pump(5000);
  }
}

void check_water_quality(){
  int sensorValue = analogRead(A0);
  int turbidity = map(sensorValue, 0, 777, 100, 0);

  if (turbidity > 30){
    // TODO: Send response for the admin to save the turbidity value
    Serial.println("Water Quality Failed");
    Serial.print("Turbidity: ");
    Serial.println(turbidity);
    digitalWrite(t2bpump, HIGH);
    delay(10000);
    digitalWrite(t2bpump, LOW);
  } else {
    Serial.println("Water Quality Passed");
    Serial.print("Turbidity: ");
    Serial.println(turbidity);
    digitalWrite(t2apump, HIGH);
    delay(5000);
    digitalWrite(t2apump, LOW);
  }
}

void check_tank1(){
  int tank1_full = digitalRead(tank1_lvl);
  if (tank1_full) {
    open_t1pump();
  }
}

void open_t1pump(){
  digitalWrite(t1pump, HIGH);
  t1pumpStartTime = millis();
  t1open = true;
}

void update_t1pump(int duration) {
  if (t1open && millis() - t1pumpStartTime >= duration) {
    digitalWrite(t1pump, LOW);
    t1open = false;
  }
}

void open_t2bpump(int duration){
  digitalWrite(t2bpump, HIGH);
  delay(duration);
  digitalWrite(t2bpump, LOW);
}

void open_t2apump(int duration){
  digitalWrite(t2apump, HIGH);
  delay(duration);
  digitalWrite(t2apump, LOW);
}

void open_rPump(int response_val) {
  int duration = response_val * 1000;
  digitalWrite(rPump, HIGH);
  delay(duration);
  digitalWrite(rPump, LOW);
}

void check_water_level(){
  int full = digitalRead(level1);
  int empty = digitalRead(level2);

  if (full){
    Serial.println("Tank 3 is full!");
  } else if (empty) {
    Serial.println("Tank 3 is empty!");
  } else {
    Serial.println("Tank 3 is neither Full nor Empty");
  }
}