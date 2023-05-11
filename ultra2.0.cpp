#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>
#include <stdlib.h>

using namespace std;

const int trig = 5;  //declare variables for pins
const int echo = 6;
long start = 0;
long stop = 0;
int dist = 0;

int main () {
	
	wiringPiSetupGpio();
	pinMode(trig, OUTPUT);
	pinMode(echo, INPUT);
	
	cout << "Ultrasonic sensors ready..." << endl;
	
	
	
	while(1) {
		digitalWrite(trig, LOW); //clear trig pin
		digitalWrite(trig, HIGH);
		delayMicroseconds(11);
		digitalWrite(trig, LOW);
		
		while(digitalRead(echo) == 0) {
			start = micros();
				while(digitalRead(echo) == 1) {
					stop = micros() - start;
					if(digitalRead(echo) == 0) {
						break;
					}
				}
			dist = stop / 58; //distance in cm 
			if(digitalRead(echo) == 0) {
				break;
			}
		}
		cout << dist << endl;
		//delay(500);
	}
	return 0;
}
