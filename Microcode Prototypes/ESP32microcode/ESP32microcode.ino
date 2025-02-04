#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

int BottleCount = 0;
int litersReceive = 0;
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

  // buttons
  btn_start_state = digitalRead(btn_start);

  if (idle){
    lcd.setCursor(0, 0);
    lcd.print("  Welcome to WEWO   ");
    lcd.setCursor(0, 3);
    lcd.print("Press once to start");
  }

  if (btn_start_state == LOW) {
    idle = false;
    checkInsertedObject();
    return;
  }

  delay(100);

  if (!idle) {
    checkInsertedObject();
    return;
  }

  // We put this below so that it wont run again while idle is true
  // lcd.setCursor(0, 0);
  // lcd.print("  Welcome to WEWO   ");
  // lcd.setCursor(0, 3);
  // lcd.print("Press once to start");

}

void checkInsertedObject() {
  bool objectDetected = false;
  stopper.write(0);
  decision.write(180);
  delay(100);

  lcd.setCursor(0,3);
  lcd.print("Hold button if done.");
  printLitersReceived();

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

    delay(300);

    Serial.println(distanceCm);

    // Ultrasonic detection values
    if (distanceCm < 10.0 || distanceCm > 200 && distanceCm != 0.0) {
      BottleCount += 1;

      // this is where the object detection will occur
      lcd.setCursor(0,0);
      lcd.print("Processing object...");
      int size = identifySize();

      // check for 3 and 4
      // '3' means not bottle, '4' means camera failure, and '5' means serial connection error
      if (size == 3 || size == 4) {
        lcd.setCursor(0,2);
        lcd.print("                    ");
        lcd.setCursor(0,2);
        lcd.print("Img Process Failed");
      } else if(size == 5) {
        lcd.setCursor(0,2);
        lcd.print("                    ");
        lcd.setCursor(0,2);
        lcd.print("Serial Con Error");
      } else {
        lcd.setCursor(0,2);
        lcd.print("                    ");
        lcd.setCursor(0,2);
        lcd.print(sizes[size]);

        int liters = sizeToLiters(size);
        if (liters != -1){
          litersReceive += liters;
        }

        // move decision servo slowly
        for (int i = 180; i > 35; i -= 1){
          decision.write(i);
          delay(10);
        }
        dec_open = true;
      }

      delay(100);
      
      // clear size display on lcd
      lcd.setCursor(0,2);
      lcd.print("                    ");

      // open/close stopper servo
      for (int i = 0; i < 90; i += 1){
        stopper.write(i);
        delay(10);
      }

      delay(100);

      for (int i = 90; i > 0; i -= 1){
        stopper.write(i);
        delay(10);
      }
      delay(100);

      // check if decision servo is open. if it does, close it
      if(dec_open){
        for (int i = 35; i < 180; i += 1){
          decision.write(i);
          delay(10);
        }
        dec_open = false;
        delay(100);
      }

      //printBottleCount();
      printLitersReceived();
      objectDetected = true;
    }
  }
}


void printLitersReceived() {
    lcd.setCursor(0,0);
    lcd.print("Please Insert Bottle");
    lcd.setCursor(0, 1);
    lcd.print("Total liters: ");
    lcd.setCursor(14, 1);
    lcd.print(litersReceive);
}


bool check_if_done() {
  btn_start_state = digitalRead(btn_start);
  if (btn_start_state == LOW) {
    Serial.println("TOTAL LITERS:" + String(litersReceive)); // send message to rpi, rpi will send to arduino to open water pumper
    printLitersReceived();
    idle = true;
    // clear LCD
    lcd.setCursor(0,0);
    lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("Thank you");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("                    ");

    litersReceive = 0;
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
    case 2:
      return 1;
    default:
      return -1;
  }
}