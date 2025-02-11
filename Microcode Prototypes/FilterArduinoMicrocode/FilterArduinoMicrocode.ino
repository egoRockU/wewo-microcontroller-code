
// pins
int t1pump = 3, t2bpump = 4, t2apump = 5, rPump = 9;
int t3Level = 11, tank1_lvl = 12;

// pump delays
int tank2pumpDelay = 1000;

// StartTime
unsigned long t1pumpST = 0;
unsigned long t2bpumpST = 0;
unsigned long t2apumpST = 0;
unsigned long rPumpST = 0;

// States
bool t1open = false;
bool t2bopen = false;
bool t2aopen = false;
bool rPumpOpen = false;

// duration
int rPumpDuration = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(t1pump, OUTPUT);
  pinMode(t2bpump, OUTPUT);
  pinMode(t2apump, OUTPUT);
  pinMode(rPump, OUTPUT);
  pinMode(t3Level, INPUT);
  pinMode(tank1_lvl, INPUT);
  Serial.begin(115200);
}

void loop() {
  // listen to serial connection
  while(Serial.available() == 0){

    check_tank1();

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

        if (value == "OPEN TANK 1"){
          open_pump(t1pump, &t1pumpST, &t1open);
        }
        else if (value == "CHECK WATER QUALITY"){
          check_water_quality();
        }
        else if (value == "CHECK WATER LEVEL"){
          check_water_level();
        }
        else if (label == "res:" && isDigit(value[0])) {
          rPumpDuration = 0; //just to clear previous duration
          int response_val = value.toInt();
          open_rPump(response_val);
        }
      }
    }

    // update states
    update_pump(t1pump, &t1pumpST, &t1open, 5000);
    update_pump(t2bpump, &t2bpumpST, &t2bopen, 10000);
    update_pump(t2apump, &t2apumpST, &t2aopen, 5000);
    update_pump(rPump, &rPumpST, &rPumpOpen, rPumpDuration);
  }
}

void check_water_quality(){
  int sensorValue = analogRead(A0);
  int turbidity = map(sensorValue, 0, 777, 100, 0);

  if (turbidity > 30){
    // TODO: Send response for the admin to save the turbidity value
    Serial.print("Water Quality Failed! | Turbidity: ");
    Serial.println(turbidity);
    open_pump(t2bpump, &t2bpumpST, &t2bopen);
  } else {
    Serial.print("Water Quality Passed! | Turbidity: ");
    Serial.println(turbidity);
    open_pump(t2apump, &t2apumpST, &t2aopen);
  }
}

void check_tank1(){
  int tank1_full = digitalRead(tank1_lvl);
  if (tank1_full) {
    open_pump(t1pump, &t1pumpST, &t1open);
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

void open_rPump(int response_val) {
  int duration = response_val * 1000;
  open_pump(rPump, &rPumpST, &rPumpOpen);
  rPumpDuration = duration;
}

void check_water_level(){
  int empty = digitalRead(t3Level);

  if (empty){
    Serial.println("Tank 3 is empty! | res: 1");
  } else {
    Serial.println("Tank 3 is NOT empty | res: 0");
  }
}