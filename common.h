#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>
#include <string>
#include <vector>

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <math.h>
#include <cmath>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#include "SHMRequestChannel.h"

using namespace std;

#define MAX_MESSAGE 4096 // maximum buffer size for each message

typedef char byte_t;

// different types of messages used in the functions
enum MESSAGE_TYPE {UNKNOWN_MSG, ULTRASONICDATA_MSG, MagData_MSG, PHOTO_MSG, OPEN_MSG, QUIT_MSG};

/*Helpful functions for debugging and other uses*/
//Causes a full exit immediately
void EXITONERROR();
//Causes a full exit immediately with a printed error message
void EXITONERROR(string message);
//A custom split function with a custom delimiter
std::vector<std::string> split (std::string line, char separator);
//Allows to cheack equals with a given error value where degree 
//is the percent diffrence between the two double values
bool nonpreciseEquals(double val1, double val2, int degree);

//Used when there is a Ultra Sonic data componet to be worked on
class ultraSonicData {
public:
    MESSAGE_TYPE mtype;
    int sensor;
    double value;

    ultraSonicData (int _sensor, double _value) {
        mtype = ULTRASONICDATA_MSG;
        sensor = _sensor;
        value = _value;
    }
};

//Used when there is a magnet data componet to be worked on
class magData{
public:
    MESSAGE_TYPE mtype;
    double value;
    double value2;

    magData(double _value, double _value2){
        mtype = MagData_MSG;
        value = _value;
        value2 = _value2;
    }
};

//Used when there is a photo sensor error or large exposher to be seen
class photoData{
public:
    MESSAGE_TYPE mtype;
    int sensor;
    bool alarm;

    photoData(int _sensor, bool _value){
        mtype = PHOTO_MSG;
        alarm = _value;
    }
};

//This is what starts the sharded memory structure
class openMessage{
public:
    MESSAGE_TYPE mtype;

    openMessage(){
        mtype = OPEN_MSG;
    }
};

//This is what ends the sharded memory structure
class quitMessage{
public:
    MESSAGE_TYPE mtype;

    quitMessage(){
        mtype = QUIT_MSG;
    }
};

//print functions
void printUltraSonicdata(ultraSonicData* m);

void printMagdata(magData* m);

void printPhotodata(photoData* m);

//Thread Functions - NOT INTENDED FOR MANUAL USE would work fine but there are
//for a thread to be deathched
void thredReader (SHMRequestChannel* channel, char* sendOut);

//This allows you to cast any message and it be properly written into
//the channel this can cause blocking if a read request is not ready OR
//a thread function has been initilized on the other side
void write_request (SHMRequestChannel* rc, char* _request);

//This smartly reads the data in a channel allowing any data message to come out
//the buffer sent should be of MAX_MESSAGE size
bool read_request (SHMRequestChannel* rc, char* _request);

//Open SHM Channel
//These two functions call the threading componets making the user have to do minimal work
//for maximum gain of use use these for guranteed no issue, on the condition the other side
//connects to do this the multiprogrammed side must give the same name to their structure
//*******
SHMRequestChannel* process_newReciving_request (char* name, char* targetReturnBuffer);

SHMRequestChannel* process_newSending_request(char* name);
//*******

//Write Commands*******
//These commands will write any single given data of the type they say
int write_UltraSonicData_request (SHMRequestChannel* rc, ultraSonicData* toSend);

int write_magData_request (SHMRequestChannel* rc, magData* toSend);

int write_photoData_request (SHMRequestChannel* rc, photoData* toSend);

//This command instaly writes and closes the channel
//******* THIS MUST BE CALLED BEFORE YOUR PROGRAM EXITS *******
//BUT it only needs to be called once by one of the two sides
int write_quit_request(SHMRequestChannel* rc);

//These are for opening a certain type of channel these are for a black box design
//with earlier functions so they do not need to be used in most circumstances
int write_open_request_ultraSonic(SHMRequestChannel* rc);

int write_open_request_mag(SHMRequestChannel* rc);

int write_open_request_photo(SHMRequestChannel* rc);

//Read Commands
//These are for opening a certain type of channel these are for a black box design
//with earlier functions so they do not need to be used in most circumstances
void read_ultraSonicData_request (SHMRequestChannel* rc, ultraSonicData* point);

void read_magData_request (SHMRequestChannel* rc, magData* point);

void read_photoData_request (SHMRequestChannel* rc, photoData* point);

void read_open_request(SHMRequestChannel* rc, char* buff);

#endif