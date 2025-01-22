#ifndef MOVE_SERVO_H
#define MOVE_SERVO_H

#include <Arduino.h>
#include <Servo.h>

void move_servo(Servo servo, int starting_pos, int direction, int stop, int speed);

#endif