#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>
#include <stdlib.h>
#include <string>

using namespace std;

const int trig1 = 5;  //declare variables for pins
const int echo1 = 6;   //sensors
const int trig2 = 20;
const int echo2 = 21;
const int trig3 = 14;
const int echo3 = 15;
const int trig4 = 13;
const int echo4 = 24;
const int trig5 = 8;
const int echo5 = 7;

long start1 = 0;   //calc vars
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
int sensor = 0; //sensor select

int main () {

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

	cout << "Ultrasonic sensors ready..." << endl;



	while(1) {
		//cout << "Select sensor to gather measurement: " << endl;
		//cin >> sensor; //gather sensor input
		//keep looping for each input, act as passing sensor for read


			digitalWrite(trig1, LOW); //clear trig pin
			digitalWrite(trig1, HIGH);

			digitalWrite(trig2, LOW);
			digitalWrite(trig2, HIGH);

			digitalWrite(trig3, LOW);
			digitalWrite(trig3, HIGH);

			digitalWrite(trig4, LOW);
			digitalWrite(trig4, HIGH);

			delayMicroseconds(10);

			digitalWrite(trig1, LOW);
			digitalWrite(trig2, LOW);
			digitalWrite(trig3, LOW);
			digitalWrite(trig4, LOW);

			while(digitalRead(echo1) == 0) {
				start1 = micros();

					while(digitalRead(echo1) == 1) {
						stop1 = micros() - start1;
						if(digitalRead(echo1) == 0) {
							break;
						}
					}

				dist1 = stop1 / 58; //distance in cm
				//wrieToShared+
				if(digitalRead(echo1) == 0) {
					break;
				}
			}

			while(digitalRead(echo2) == 0) {
				start2 = micros();

					while(digitalRead(echo2) == 1) {
						stop2 = micros() - start2;
						if(digitalRead(echo2) == 0) {
							break;
						}
					}

				dist2 = stop2 / 58; //distance in cm
				if(digitalRead(echo2) == 0) {
					break;
				}
			}

			while(digitalRead(echo3) == 0) {
				start3 = micros();

					while(digitalRead(echo3) == 1) {
						stop3 = micros() - start3;
						if(digitalRead(echo3) == 0) {
							break;
						}
					}

				dist3 = stop3 / 58; //distance in cm
				if(digitalRead(echo3) == 0) {
					break;
				}
			}

			while(digitalRead(echo4) == 0) {
				start4 = micros();

					while(digitalRead(echo4) == 1) {
						stop4 = micros() - start4;
						if(digitalRead(echo4) == 0) {
							break;
						}
					}

				dist4 = stop4 / 58; //distance in cm
				if(digitalRead(echo4) == 0) {
					break;
				}
			}

			cout << "Distance1: " << dist1 << " cm" << endl;
			cout << "Distance2: " << dist2 << " cm" << endl;
			cout << "Distance3: " << dist3 << " cm" << endl;
			cout << "Distance4: " << dist4 << " cm" << endl;

	}
	return 0;
}
