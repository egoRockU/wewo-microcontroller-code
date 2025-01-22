#include "move_servo.h"

/**
move_servo is a function to control the speed of a servo

all parameters are integer

servo = the servo object you wish to move
starting_pos = the starting position(or angle) of servo (0-180)
direction = 0 or 1. 0 for descending(180->0) and 1 for ascending (0->180)
stop = the stopping position(or angle) of the servo
speed = the value for delay. higher value is slower
**/
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