#include <Servo.h>
#include <move_servo.h>

Servo ser;

int pin = 9;
int starting_pos = 0;

void setup() {
  // servo initializations
  ser.attach(pin);
  ser.write(starting_pos);
}

void loop() {
  /**
    move_servo is a function to control the speed of a servo

    all parameters are integer except for servo

    servo = the servo object you wish to move
    starting_pos = the starting position(or angle) of servo (0-180)
    direction = 0 or 1. 0 for descending(180->0) and 1 for ascending (0->180)
    stop = the stopping position(or angle) of the servo
    speed = the value for delay. higher value is slower
  **/
  
  move_servo(ser, 180, 0, 35, 10);
}
