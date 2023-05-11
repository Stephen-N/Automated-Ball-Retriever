#include <stdio.h>
#include <iostream>   //including libraries
#include <wiringPi.h>
#include <time.h>

using namespace std;

const int photo_detect = 26;
int obstacle = 0;           //variable to pass if navigation 
							//hazard detected 
int main () { 
	
	wiringPiSetupGpio ();
	pinMode (photo_detect, INPUT);
	
	cout << "Photo sensor ready..." << endl;
	cout << " " << endl;
	
	while (1) {
		
		if (digitalRead(photo_detect) == 1) { //navigation stick in place
			
			obstacle = 0; //no hazard detected
			
			cout << "No hazard detected..." << endl;
			//cout << obstacle << endl;
			
		}
		
		else if (digitalRead(photo_detect) == 1) { //navigation hazard detected
			
			obstacle = 1; //hazard detcted, pass int
			
			cout << "Hazard detected..." << endl;
			//cout << obstacle << endl;
		}

	}
	
	return 0;
}

