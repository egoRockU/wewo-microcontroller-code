const int trigPin = 7;
const int echoPin = 8;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

void get_average_cm();
float get_cm();

void setup() {
  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.println("Write 'start' to calculate average distance value");
}

void loop() {
  if (Serial.available()){
    String input = Serial.readStringUntil('\n');
    if (input == "start"){
      get_average_cm();
    }
  }
}

void get_average_cm(){
  String loading[8] = {"|", "/", "-", "\\", "|", "/", "-", "\\"};
  int load_sign = 0;
  float average = 0;
  int cycles = 10;

  for (int i = 0; i < cycles; i++){
    if (load_sign <= 7){
      Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
      Serial.println("Calculating " + loading[load_sign]);
      load_sign++;
    } else {
      load_sign = 0;
    }

    float distance = get_cm();
    average += distance;

    delay(1000);    
  }

  average = average / cycles;
  Serial.println("Average Distance (CM): " + String(average));
  Serial.println("\n");
  Serial.println("Write 'start' to calculate average distance value");
}

float get_cm(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  return distanceCm;
}

