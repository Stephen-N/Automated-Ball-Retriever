#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <wiringPi.h>
#include <iostream>  //including libraries
#include <string>

#include "common.h"

using namespace std;

int select = 0;
const int IN1 = 4;  //variable for pinmode logic control
const int IN2 = 23;
const int IN3 = 25;
const int IN4 = 12




//****WHEN SWITCHING FWD->REVERSE**** BRAKE > 0.1S
//***CANNOT REVERSE WHEN MOTOR IS NOT STOPPED

void brake() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);


}

void full_speed_forward() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

}

void full_speed_reverse() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

}

void reverse_speed() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

}

void forward_to_speed() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

}

void floating() {

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

}

 // 1 = BRAKE
 // 2 = float
 // 3 = fwd to speed
 // 4 = rev speed
 // 5 = full speed fwd
 // 6 = full speed rev

int main() {

  wiringPiSetupGpio(); //set up gpio

  pinMode(IN1, OUTPUT); //pinmode for IN pins
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  while(1) {

    cout << "Enter command: " << endl;
    cin >> select;

    if (select == 1) {
      brake();
      sleep(1);

    }

    else if (select == 2) {
      floating();
    }

    else if (select == 3) {
      forward_to_speed();
    }

    else if (select == 4) {
      brake();
      sleep(1);
      reverse_speed();
    }

    else if (select == 5) {
      full_speed_forward();
    }

    else if (select == 5) {
      brake();
      delayMicroseconds(0.0000002);
      full_speed_reverse();
    }

  }

  return 0;
}
