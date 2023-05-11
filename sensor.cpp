#include "common.h"

using namespace std;
/*
NOTES FOR THE SHM request
--For more notes view common.h it also cointains other goodies that can help in a variety of circumstances
								--really, just look at it :)
												--PS I put *** for the important stuff

--Also don't forget the printDATATYPE functions they are clean and formatted


example of a send request
SHMRequestChannel* YOURNAMEHERE = process_newSending_request(argv); //use the string I pass you for sensors
Then to actually write the data
ultraSonicData* OTHERNAMEHERE = new ultraSonicData(sensorNumber, sensorValue); 	//where sensorNumber is the number from argv, 
																//and sensor value is the value you want to send


FOR WRITING there are two options depending of if you want to cast OR use the particular function
write_request(YOURNAMEHERE, (char*)mydatamessage)
OR
write_UltraSonicData_request(YOURNAMEHERE, mydatamessage)

SOMEONE MUST WRITE CLOSE AT THE END -- I will do it tho :)
write_quit_request(YOURNAMEHERE);

Now to do a read reqest
char* multiBuf = new char[sizeof(MESSAGE_TYPE)];
SHMRequestChannel* YOURNAMEHERE = process_newReciving_request("sensor_#", multiBuf);
then use multiBuf when you want and cast the data to it in order to save it
ultraSonicData* m = (ultraSonicData*) multiBuf;
--as a note there are other read options and steps that can be taken check documentation for more
*/


bool compareStringChar(string var1, char* var2){
	bool isSame = true;
	try{
		for (size_t i = 0; i < var1.length(); i++)
		{
			isSame = isSame && (var1[i]==var2[i]);
		}
		
	} catch (exception e) {
		return false;
	}
	return isSame;
}


int main (int argc, char* argv[]) {//example input for the argv"ultasonic_1"

	if(isSame("ultraSound", argv[1])){ 
		cout << "doing ultra soudn stuff\n";
		cout << "my sensor number is:\t" << argv[0] << endl;
	} else if (argv[1] == "photo"){
		//do stuff
	}

	return 0;
}