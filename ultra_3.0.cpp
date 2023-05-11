#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <wiringPi.h>

#include <iostream>  //including libraries
#include <string>

#include "common.h"

using namespace std;

const int trig1 = 5;  // declare variables for pins
const int echo1 = 6;  // sensors
const int trig2 = 20;
const int echo2 = 21;
const int trig3 = 14;
const int echo3 = 15;
const int trig4 = 13;
const int echo4 = 24;
const int trig5 = 8;
const int echo5 = 7;
const double max_dist = 125; //max distance

long start1 = 0;  // calc vars
long stop1 = 0;
int dist1 = 0;
long start2 = 0;
long stop2 = 0;
int dist2 = 0;
long start3 = 0;
long stop3 = 0;
int dist3 = 0;
long start4 = 0;
long stop4 = 0;
int dist4 = 0;
long start5 = 0;
long stop5 = 0;
int dist5 = 0;

void ultra1(int sensorNumber, SHMRequestChannel* chan) {
    while (true) {
        digitalWrite(trig1, LOW);  // clear trig pin
        digitalWrite(trig1, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig1, LOW);

        while (digitalRead(echo1) == 0) {
            start1 = micros();

            while (digitalRead(echo1) == 1) {
                stop1 = micros() - start1;
                if (digitalRead(echo1) == 0) {
                    break;
                }
            }

            dist1 = stop1 / 58;  // distance in cm
            if (dist1 >= max_dist) {
              dist1 = -1;
            }
            // wrieToShared+
            if (digitalRead(echo1) == 0) {
                break;
            }
        }
        ultraSonicData* u1 = new ultraSonicData(sensorNumber, dist1);
        write_UltraSonicData_request(chan, u1);
    }
}

void ultra2(int sensorNumber, SHMRequestChannel* chan) {
    while (true) {
        digitalWrite(trig2, LOW);  // clear trig pin
        digitalWrite(trig2, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig2, LOW);

        while (digitalRead(echo2) == 0) {
            start2 = micros();

            while (digitalRead(echo2) == 1) {
                stop2 = micros() - start2;
                if (digitalRead(echo2) == 0) {
                    break;
                }
            }

            dist2 = stop2 / 58;  // distance in cm
            if (digitalRead(echo2) == 0) {
                break;
            }
            if (dist2 >= max_dist) {
              dist2 = -1;
            }
        }
        ultraSonicData* u2 = new ultraSonicData(sensorNumber, dist2);
        write_UltraSonicData_request(chan, u2);
    }
}

void ultra3(int sensorNumber, SHMRequestChannel* chan) {
    while (true) {
        digitalWrite(trig3, LOW);  // clear trig pin
        digitalWrite(trig3, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig3, LOW);

        while (digitalRead(echo3) == 0) {
            start3 = micros();

            while (digitalRead(echo3) == 1) {
                stop3 = micros() - start3;
                if (digitalRead(echo3) == 0) {
                    break;
                }
            }

            dist3 = stop3 / 58;  // distance in cm
            if (digitalRead(echo3) == 0) {
                break;
            }
            if (dist3 >= max_dist) {
              dist3 = -1;
            }
        }
        ultraSonicData* u3 = new ultraSonicData(sensorNumber, dist3);
        write_UltraSonicData_request(chan, u3);
    }
}

void ultra4(int sensorNumber, SHMRequestChannel* chan) {
    while (true) {
        digitalWrite(trig4, LOW);  // clear trig pin
        digitalWrite(trig4, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig4, LOW);

        while (digitalRead(echo4) == 0) {
            start4 = micros();

            while (digitalRead(echo4) == 1) {
                stop4 = micros() - start4;
                if (digitalRead(echo4) == 0) {
                    break;
                }
            }

            dist4 = stop4 / 58;  // distance in cm
            if (digitalRead(echo4) == 0) {
                break;
            }
            if (dist4 >= max_dist) {
              dist4 = -1;
            }
        }
        ultraSonicData* u4 = new ultraSonicData(sensorNumber, dist4);
        write_UltraSonicData_request(chan, u4);
    }
}

void ultra5(int sensorNumber, SHMRequestChannel* chan) {
    while (true) {
        digitalWrite(trig5, LOW);  // clear trig pin
        digitalWrite(trig5, HIGH);
        delayMicroseconds(10);
        digitalWrite(trig5, LOW);

        while (digitalRead(echo5) == 0) {
            start5 = micros();

            while (digitalRead(echo5) == 1) {
                stop5 = micros() - start5;
                if (digitalRead(echo5) == 0) {
                    break;
                }
            }

            dist5 = stop5 / 58;  // distance in cm
            // wrieToShared+
            if (digitalRead(echo5) == 0) {
                break;
            }
            if (dist5 >= max_dist) {
              dist5 = -1;
            }
        }
        ultraSonicData* u5 = new ultraSonicData(sensorNumber, dist5);
        write_UltraSonicData_request(chan, u5);
    }
}

int main() {
    wiringPiSetupGpio();
    pinMode(trig1, OUTPUT);
    pinMode(echo1, INPUT);
    pinMode(trig2, OUTPUT);
    pinMode(echo2, INPUT);
    pinMode(trig3, OUTPUT);
    pinMode(echo3, INPUT);
    pinMode(trig4, OUTPUT);
    pinMode(echo4, INPUT);
    pinMode(trig5, OUTPUT);
    pinMode(echo5, INPUT);

    SHMRequestChannel* chan = process_newSending_request("writer");

    ultra1(1, chan);
    ultra2(2, chan);
    ultra3(3, chan);
    ultra4(4, chan);
    ultra5(5, chan);

    wait();
}
