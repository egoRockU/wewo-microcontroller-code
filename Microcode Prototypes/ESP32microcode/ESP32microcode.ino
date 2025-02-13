#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

int BottleCount = 0;
int litersReceive = 0;
bool idle = true;
bool dec_open = false;
bool stopper_open = true;

LiquidCrystal_I2C lcd(0x27, 20, 4);

//servo
int stopper_pin = 25, decision_pin = 5;
Servo stopper, decision;
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
int sizes_value[] = {3, 2, 1};

void setup() {
  Serial.begin(115200);

  // lcd setup
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // servo setup
  stopper.attach(stopper_pin);
  stopper.write(90); // 90 is open
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
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    lcd.print("Please Wait...");
    check_sizes_values();
    // tank3 checking, delete when testing
    if (is_tank3_empty()){
      idle = true;
      lcd.setCursor(0, 3);
      lcd.print("                    ");
      lcd.setCursor(0, 3);
      lcd.print("WEWO is empty!");
      // // open/close stopper servo
      // for (int i = 0; i < 90; i += 1){
      //   stopper.write(i);
      //   delay(10);
      // }

      // delay(100);

      // for (int i = 90; i > 0; i -= 1){
      //   stopper.write(i);
      //   delay(10);
      // }
      // delay(100);

      delay(5000);
      return;
    }
    checkInsertedObject();
    return;
  }

  delay(100);

  if (!idle) {
    checkInsertedObject();
    return;
  }

}

void checkInsertedObject() {
  bool objectDetected = false;

  if (stopper_open){
    stopper.write(90);
    for (int i = 90; i > 0; i -= 1){
      stopper.write(i);
      delay(10);
    }
    delay(100);
    stopper_open = false;
  }

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
      stopper_open = false;
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
    lcd.setCursor(0,2);
    lcd.print("                    ");
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

    // open stopper once again
    for (int i = 0; i < 90; i += 1){
      stopper.write(i);
      delay(10);
    }
    stopper_open = true;
    delay(100);

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

// run this before collecting bottles
void check_sizes_values() {
  Serial.println("Req: Check Pumper Values");
  //expected response example: "res: Large: 4 | Medium: 3 | Small: 2"

  while (true) {
    if (Serial.available() > 0) {
      String response = Serial.readString();
      if (response.length() > 0) {
        response.trim();
        int separatorIndex = response.indexOf(":");
        String label = response.substring(0, separatorIndex + 1);
        label.toUpperCase();
        if (label == "RES:") {
          change_sizes_value(response);
        } else if (label == "ERROR:") {
          Serial.println("Error Changing Size values, using the default for now!");
        }
        
        return;
      }
    }
  }

}

int is_tank3_empty() {
  Serial.println("Req: Check Tank 3");
  // expected response example: "res: 1" if empty or "res: 0" if not empty
  
  while(true){
    if (Serial.available() > 0){
      String response = Serial.readString();
      if (response.length() > 0){
        response.trim();
        int separatorIndex = response.indexOf(":");
        String label = response.substring(0, separatorIndex + 1); 
        String value = response.substring(separatorIndex + 1);

        // Remove extra whitespace
        label.trim(); 
        value.trim();
        label.toLowerCase();
        int is_empty = value.toInt();
        if (label == "res:") {
          return is_empty;
        }
      }
    }
  }
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
      return sizes_value[0];
    case 1:
      return sizes_value[1];
    case 2:
      return sizes_value[2];
    default:
      return -1;
  }
}

void change_sizes_value(String data) {
    int largeValue = extractValue(data, "Large:");
    int mediumValue = extractValue(data, "Medium:");
    int smallValue = extractValue(data, "Small:");

    sizes_value[0] = largeValue;
    sizes_value[1] = mediumValue;
    sizes_value[2] = smallValue;

    Serial.println("Sizes Values Updated!");
}

int extractValue(String data, String key) {
    int index = data.indexOf(key);
    if (index != -1) {
        int start = index + key.length(); // Move to the number part
        int end = data.indexOf('|', start); // Find the next separator
        if (end == -1) end = data.length(); // If last value, take till end
        return data.substring(start, end).toInt(); // Convert to integer
    }
    return 0; // Default if key not found
}