int rPump = 8;
int t3Level = 10;

unsigned long rPumpST = 0;
bool rPumpOpen = false;
int rPumpDuration = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(rPump, OUTPUT);
  pinMode(t3Level, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available() == 0){
    String response = Serial.readStringUntil('\n');
    if (response) {
      response.trim();
      int separatorIndex = response.indexOf(":");
      if (separatorIndex != -1){
        String label = response.substring(0, separatorIndex + 1); 
        String value = response.substring(separatorIndex + 1);

        // Remove extra whitespace
        label.trim(); 
        value.trim();
        label.toLowerCase();
        value.toUpperCase();

        if (label == "provide res:" && isDigit(value[0])) {
          rPumpDuration = 0; //just to clear previous duration
          int response_val = value.toInt();
          open_rPump(response_val);
        }
      }
    }

    update_pump(rPump, &rPumpST, &rPumpOpen, rPumpDuration);
  }
}


void open_rPump(int response_val) {
  int duration = response_val * 1000;
  open_pump(rPump, &rPumpST, &rPumpOpen);
  rPumpDuration = duration;
}

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