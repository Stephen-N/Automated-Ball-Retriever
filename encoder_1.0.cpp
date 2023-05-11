#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

using namespace std;

//encoder 6 pulses/cirlce (rotation?)
//if chan A leads, CW rotation
//chan B leader, CCW rotation




int main () {
	
	const int chanA = 14;
	const int chanB = 15;
	clock_t start = clock();
	double n = 0;
	double speed = 0;
	double true_speed = 0;

	wiringPiSetupGpio();  //init library, set encoder pins to gpio
	pinMode(chanA, INPUT);
	pinMode(chanB, INPUT);
	
	//samplting period = t
	//pulses counted during sampling period = n
	//avg time for pulse = t/n
	//pulses/revolution = N = 6
	//w = ang speed = 2pin/Nt (rad/s)
	//shaft radius = 24mm?
	while (1) {
		
		if ( (double) (clock() - start) / CLOCKS_PER_SEC >= 2) { // t = 1s
		speed = (2 * 3.141592654 * n) / (6); //rad/s
		true_speed = speed * 0.024; //m/s
		cout << n << " n" << endl;
		cout << speed << " angular" << endl;
		cout << true_speed << " true speed" << endl << endl;
		cout.flush();
		start = clock();
		n = 0;
		}
		
		if (digitalRead(chanA) == 1){
			n++; //increment pulse counter
		}
		
		sleep(0.1);
		
	}
	
	return 0;
}
