#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

//angular velocity w = ^0/^t
//linear velocity v = r*w
//have angular deplacement inside while(1)
//establish time deplacement, calculate angular velocity then use shaft length
//to calculate linear velocity

long encoder;
int stateA = 0;
int stateB = 0;
float angle = 0;
double omega = 0;
const double radius = 0.0127; //0.0127 m radius
double velocity = 0;

const int wireA = 17;
const int wireB = 27;

void A() {

  if (stateA == stateB) {
    encoder++;
  }
  stateA = digitalRead(wireA);
  return;
}

void B() {

  if (stateA == stateB) {
    encoder--;
  }
  stateB = digitalRead(wireB);
  return;
}

int main() {

  wiringPiSetupGpio();

  wiringPiISR(wireA, INT_EDGE_BOTH, &A);
  wiringPiISR(wireB, INT_EDGE_BOTH, &B);

  while(1) {
    clock_t start = clock(); //^t start time
    angle = encoder * 0.3;
    cout << "Angle is: " << angle << endl;

    if ((double) (clock() - start) / CLOCKS_PER_SEC >= 0.05) { //every 2 seconds, or ^t
      //calculate omega
      omega = angle / 0.05; //w = 0 / 2(s)
      cout << "Angular displacement: " << omega << endl;
      //calculate linear velocity
      velocity = omega * radius; //v = w*r
      cout << "Linear velocity: " << velocity << endl;
    }

  }

  return 0;
}
