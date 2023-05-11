#ifndef _SHMREQUESTCHANNEL_H_
#define _SHMREQUESTCHANNEL_H_

#include <iostream>
#include <fstream>
#include <thread>
#include <cstring>
#include <string>
#include <vector>

#include <semaphore.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>


enum Side {RECIVING_SIDE, SENDING_SIDE};

class SHMQueue {
private:
    std::string name;
    int length;

    char* segment;
    sem_t* recv_done;
    sem_t* send_done;
    
public:
    SHMQueue(const std::string _name, int _length);
    ~SHMQueue();

    int shm_receive (void* msgbuf, int msgsize);
    int shm_send (void* msgbuf, int msgsize);
};

class SHMRequestChannel{
    private:
        std::string my_name;
        Side my_side;
        SHMQueue * shmQueue1;
        SHMQueue * shmQueue2;

    public:
        SHMRequestChannel (const std::string _name, const Side _side, int _length);
        ~SHMRequestChannel();

        int cread(void* msgbuf, int bufcapacity);
        int cwrite(void* msgbuf, int msglen);

        std::string getName();
};

#endif
