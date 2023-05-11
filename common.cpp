#include "common.h"

using namespace std;

int buffercapacity = MAX_MESSAGE;
char* buffer = NULL;


//print functions
void printUltraSonicdata(ultraSonicData* m){
	cout << "Ultra Sonic Sensor Reading\n";
	cout << "type:\t" << m->mtype;
    cout << "\nsensor:\t" << m->sensor;
    cout << "\nvalue:\t" << m->value << "\n\n";

	cout.flush();
}

void printMagdata(magData* m){
	cout << "Magnetic Sensor Reading\n";
	cout << "type:\t" << m->mtype;
    cout << "\nSensor 1 reading:\t" << m->value;
    cout << "\nSensor 2 reading:\t" << m->value2 << "\n\n";

	cout.flush();
}

void printPhotodata(photoData* m){
	cout << "Photo Sensor Reading\n";
	cout << "Type:\t" << m->mtype;
    cout << "\nSensor:\t" << m->sensor;
    cout << "\nAlarm?:\t" << m->alarm << "\n\n";

	cout.flush();
}

void EXITONERROR(string message){
	perror(message.c_str());
    exit(-1);
}

void EXITONERROR(){
    perror("Critical Failure!!");
    exit(-1);
}

bool nonpreciseEquals(double val1, double val2, int degree){
	int multi = 1;
	for (size_t i = 0; i < degree; i++)
	{
		multi = multi * 10;
	}
	
	return (static_cast<int>(val1*multi)) == (static_cast<int>(val2*multi));
}

int write_UltraSonicData_request (SHMRequestChannel* rc, ultraSonicData* toSend) {
	return rc->cwrite(toSend, sizeof(ultraSonicData));
}

int write_magData_request (SHMRequestChannel* rc, magData* toSend) {
	return rc->cwrite(toSend, sizeof(magData));
}

int write_photoData_request (SHMRequestChannel* rc, photoData* toSend) {
	return rc->cwrite(toSend, sizeof(photoData));
}

int write_quit_request(SHMRequestChannel* rc){
	return rc->cwrite(new quitMessage(), sizeof(quitMessage));
}

int write_open_request_ultraSonic(SHMRequestChannel* rc){
	return rc->cwrite(new ultraSonicData(-1, -1), sizeof(ultraSonicData));
}

int write_open_request_mag(SHMRequestChannel* rc){
	return rc->cwrite(new magData(-1, -1), sizeof(magData));
}

int write_open_request_photo(SHMRequestChannel* rc){
	return rc->cwrite(new photoData(-1, 0), sizeof(photoData));
}

void read_ultraSonicData_request (SHMRequestChannel* rc, ultraSonicData* point){
    rc->cread(point, sizeof(ultraSonicData));
}

void read_magData_request (SHMRequestChannel* rc, magData* point){
    rc->cread(point, sizeof(magData));
}

void read_photoData_request (SHMRequestChannel* rc, photoData* point){
    rc->cread(point, sizeof(photoData));
}

void read_open_request(SHMRequestChannel* rc, char* buff){
	rc->cread(buff, sizeof(MESSAGE_TYPE));
}

vector<string> split (string line, char separator) {
	vector<string> result;
	while (line.size()) {
		size_t found = line.find_first_of(separator);
		if (found != std::string::npos) {
			string part = line.substr(0, found);
			result.push_back(part);
			line = line.substr(found+1);
		}
		else {
			result.push_back(line);
			break;
		}
	}
	return result;
}

void write_request (SHMRequestChannel* rc, char* _request) {
	MESSAGE_TYPE m = *((MESSAGE_TYPE*) _request);
	if (m == ULTRASONICDATA_MSG) {
		write_UltraSonicData_request(rc, (ultraSonicData*) _request);
	} else if (m == MagData_MSG){
        write_magData_request(rc, (magData*) _request);
    } else if (m == PHOTO_MSG){
        write_photoData_request(rc, (photoData*) _request);
    } else if (m == QUIT_MSG){
		write_quit_request(rc);
        delete rc;
    } else {
        printf("\n\nFAILURE BAD MESSAGE!!!\n\n");
    }
}

bool read_request (SHMRequestChannel* rc, char* _request) {
	MESSAGE_TYPE m = *((MESSAGE_TYPE*) _request);
	if (m == ULTRASONICDATA_MSG) {
		read_ultraSonicData_request(rc, (ultraSonicData*) _request);
		return true;
	} else if (m == MagData_MSG){
        read_magData_request(rc, (magData*) _request);
		return true;
    } else if (m == PHOTO_MSG){
        read_photoData_request(rc, (photoData*) _request);
		return true;
    } else if (m == QUIT_MSG){
        return false;
    } else {
		read_open_request(rc, _request);
		return true;
	}
}

SHMRequestChannel* process_newSending_request(char* name){
    SHMRequestChannel* newChannel = new SHMRequestChannel(name, SENDING_SIDE, MAX_MESSAGE);
    return newChannel;
}

SHMRequestChannel* process_newReciving_request (char* name, char* targetReturnBuffer) {
	SHMRequestChannel* data_channel = NULL;
	data_channel = new SHMRequestChannel(name, RECIVING_SIDE, buffercapacity);

	thread threadWorker(thredReader, data_channel, targetReturnBuffer);
	threadWorker.detach();
	printf("Thread open and detached\n");
	return data_channel;
}

void thredReader (SHMRequestChannel* channel, char* sendOut) {
	while (read_request(channel, sendOut)) {
		//any necissarry changes go here
	}
	cout << "Quit message recived now exiting the channel: \"" << channel->getName() << "\""<< endl;
    delete channel;
}