4#include <chrono>
#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>

using namespace std;

const int rotate = 5; //variable for tracking angular velocity through wheel rotations
const int two_pi = 2.628318; //2pi
const int pulses = 20; //pulses per revolution
const int sample = 1; //sampling period (seconds)
const int radius = 0.01285; //meters
int omega = 0; //angular velocity
double v = 0; //velocity (m/s)
int n = 0; //measured pulses
int t = 0; //timer counter

//time_t start = time(); 	//rotational speed, linear velocity
	//w = 2pi*n / tN , v = w*r

int main () {

	wiringPiSetupGpio();  //setup library
	pinMode (rotate,INPUT); //gpio 5 input

	auto now = std::chrono::steady_clock::now; //chrono stuff
	using namespace std::chrono_literals;
	auto work_duration = 5s;
	auto start = now();

	while (1){

		while((now() - start) < work_duration) { //nested infinite while loops, inner loop break and restart after 1 seconds

			if (digitalRead(rotate) == 0) { //wheel block in place

				n++; //increment pulse counter

				}

			else if (digitalRead(rotate) == 1) { //wheel block open

				n = n; //no count

				}
		}

		omega = (two_pi * n) / (sample  * pulses); //calulculate angular velocity

		v = (radius * omega);//linear velocity, m/s

		n = 0; //reset pulse counter
		cout << omega << endl;

		cout << "Velocity: " << v << " m/s" << endl;
		delayMicroseconds(1000000);
	}


	return 0;
}
