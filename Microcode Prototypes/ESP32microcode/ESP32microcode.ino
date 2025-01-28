#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

int BottleCount = 0;
int litersRecieve = 0;
bool idle = true;
bool dec_open = false;

LiquidCrystal_I2C lcd(0x27, 20, 4);

//servo
int stopper_pin = 25, decision_pin = 5;
Servo stopper;
Servo decision;
int stopper_start = 0, decision_start = 180;

// ultrasonic
const int trigPin = 32;
const int echoPin = 33;
long duration;
float distanceCm;

// button
const int btn_start = 18;
int btn_start_state = 1;

// function initialization
void checkInsertedObject();
void printBottleCount();
bool check_if_done();
int identifySize();
bool noSerialConnection();
int sizeToLiters(int size);
void move_servo(Servo servo, int starting_pos, int direction, int stop, int speed);

//value
String sizes[] = {"Large", "Medium", "Small"};

void setup() {
  Serial.begin(115200);

  // lcd setup
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // servo setup
  stopper.attach(stopper_pin);
  stopper.write(0); // 90 is open
  decision.attach(decision_pin);
  decision.write(180); //35 is open

  // ultrasonic setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // buttons setup
  pinMode(btn_start, INPUT_PULLUP);
  digitalWrite(btn_start, HIGH);
}

void loop() {
  // lcd
  lcd.setCursor(0, 0);
  lcd.print("  Welcome to WEWO   ");
  lcd.setCursor(0, 3);
  lcd.print("Press once to start");

  // buttons
  btn_start_state = digitalRead(btn_start);

  if (btn_start_state == LOW) {
    idle = false;
    checkInsertedObject();
  }

  delay(100);

  if (!idle) {
    checkInsertedObject();
  }

}

void checkInsertedObject() {
  bool objectDetected = false;
  stopper.write(0);
  decision.write(180);
  printBottleCount();
  delay(1000);

  lcd.setCursor(0,0);
  lcd.print("                    ");
  lcd.setCursor(0,3);
  lcd.print("Hold button if done.");

  objectDetected = check_if_done();
  while (!objectDetected) {

    objectDetected = check_if_done();
    if (objectDetected) { return; }

    // ultrasonic
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED/2;

    delay(1000);

    Serial.println(distanceCm);

    // Ultrasonic detection values
    if (distanceCm < 10.0 && distanceCm != 0.0) {
      BottleCount += 1;

      // this is where the object detection will occur
      lcd.setCursor(0,2);
      lcd.print("Processing object...");
      int size = identifySize();

      // check for 3 and 4
      // '3' means not bottle, '4' means camera failure, and '5' means serial connection error
      if (size == 3 || size == 4) {
        lcd.setCursor(0,0);
        lcd.print("                    ");
        lcd.setCursor(0,0);
        lcd.print("Img Process Failed");
      } else if(size == 5) {
        lcd.setCursor(0,0);
        lcd.print("                    ");
        lcd.setCursor(0,0);
        lcd.print("Serial Con Error");
      } else {
        lcd.setCursor(0,0);
        lcd.print("                    ");
        lcd.setCursor(0,0);
        lcd.print(sizes[size]);

        int liters = sizeToLiters(size);
        if (liters != -1){
          litersRecieve += liters;
        }

        move_servo(decision, 180, 0, 35, 10);
        decision.write(35);
        dec_open = true;
      }

      delay(5000);
      
      lcd.setCursor(0,2);
      lcd.print("                    ");

      // open/close stopper
      move_servo(stopper, 0, 1, 90, 10);
      stopper.write(90);
      delay(5000);
      move_servo(stopper, 90, 0, 0, 10);
      stopper.write(0);
      delay(2000);

      if(dec_open){
        move_servo(decision, 35, 1, 180, 10);
        decision.write(180);
        dec_open = false;
        delay(2000);
      }

      printBottleCount();
      gate.write(180);
      objectDetected = true;
    }
  }
}

void printBottleCount() {
    lcd.setCursor(0, 1);
    lcd.print("Bottle count: ");
    lcd.setCursor(14, 1);
    lcd.print(BottleCount);
}

bool check_if_done() {
  btn_start_state = digitalRead(btn_start);
  if (btn_start_state == LOW) {
    Serial.println("TOTAL LITERS:" + String(litersRecieve));
    gate.write(0);
    printBottleCount();
    idle = true;
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("Thank you");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("                    ");

    BottleCount = 0;
    return true;
  }
  return false;
}


int identifySize() {
  Serial.println("OBJECT DETECTED!");
  int response_val;

  if (Serial.available() != 0){
    return 5;
  }
  
  while (Serial.available()==0){
    String response = Serial.readString();
    if (response){
      // sample response: "res: 2\n"
      // where value corresponds to index in array response = ["Large", "Meduim", "Small", "Not Bottle", "Camera Failed"]
      response.trim();
      int separatorIndex = response.indexOf(":");
      if (separatorIndex != -1) {
        String label = response.substring(0, separatorIndex + 1); 
        String value = response.substring(separatorIndex + 1);

        // Remove extra whitespace
        label.trim(); 
        value.trim();
        response_val = value.toInt();
        break;
      }
    }
  }
  return response_val;
}

bool noSerialConnection(){
  if (Serial.available() != 0){
    return false;
  } else {
    return true;
  }
}

int sizeToLiters(int size){
  switch (size) {
    case 0:
      return 3;
    case 1:
      return 2;
    case 3:
      return 1;
    default:
      return -1;
  }
}

void move_servo(Servo servo, int starting_pos, int direction, int stop, int speed){

  if (direction == 0){

    if (stop >= starting_pos){
      return;
    }

    for (int i = starting_pos; i >= stop; i -= 1){
      servo.write(i);
      delay(speed);
    }
    return;

  } else if (direction == 1) {
    
    if (stop <= starting_pos){
      return;
    }

    for (int i = starting_pos; i <= stop; i += 1){
      servo.write(i);
      delay(speed);
    }
    return;

  } 
}