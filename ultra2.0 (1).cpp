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
		cout << "Select sensor to gather measurement: " << endl;
		cin >> sensor; //gather sensor input
		//keep looping for each input, act as passing sensor for read
		
		if (sensor == 1) {
			digitalWrite(trig1, LOW); //clear trig pin
			digitalWrite(trig1, HIGH);
			delayMicroseconds(10);
			digitalWrite(trig1, LOW);
		
			while(digitalRead(echo1) == 0) {
				start1 = micros();
					while(digitalRead(echo1) == 1) {
						stop1 = micros() - start1;
						if(digitalRead(echo1) == 0) {
							break;
						}
					}
				dist1 = stop1 / 58; //distance in cm 
				if(digitalRead(echo1) == 0) {
					break;
				}
			}
			cout << "***SENSOR1*** " << "Object distance : " << dist1 << " cm." << endl;
		}
		
		else if (sensor == 2) {
			digitalWrite(trig2, LOW); //clear trig pin
			digitalWrite(trig2, HIGH);
			delayMicroseconds(10);
			digitalWrite(trig2, LOW);
		
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
			cout << "***SENSOR2*** " << "Object distance : " << dist2 << " cm." << endl;
		}
		
		else if (sensor == 3) {
			digitalWrite(trig3, LOW); //clear trig pin
			digitalWrite(trig3, HIGH);
			delayMicroseconds(10);
			digitalWrite(trig3, LOW);
		
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
			cout << "***SENSOR3*** " << "Object distance : " << dist3 << " cm." << endl;
		}
		
		else if (sensor == 4) {
			digitalWrite(trig4, LOW); //clear trig pin
			digitalWrite(trig4, HIGH);
			delayMicroseconds(10);
			digitalWrite(trig4, LOW);
		
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
			cout << "***SENSOR4*** " << "Object distance : " << dist4 << " cm." << endl;
		}
		
		else if (sensor == 5) {
			digitalWrite(trig5, LOW); //clear trig pin
			digitalWrite(trig5, HIGH);
			delayMicroseconds(10);
			digitalWrite(trig5, LOW);
		
			while(digitalRead(echo5) == 0) {
				start5 = micros();
					while(digitalRead(echo5) == 1) {
						stop5 = micros() - start5;
						if(digitalRead(echo5) == 0) {
							break;
						}
					}
				dist5 = stop5 / 58; //distance in cm 
				if(digitalRead(echo5) == 0) {
					break;
				}
			}
			cout << "***SENSOR5*** " << "Object distance : " << dist5 << " cm." << endl;
		}
	}
	return 0;
}
