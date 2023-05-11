#include <sys/mman.h>

#include "SHMRequestChannel.h"

using namespace std;

SHMQueue::SHMQueue(const std::string _name, int _length) : name(_name), length(_length) {
    int fd = shm_open(name.c_str(), O_RDWR | O_CREAT, 0600);
    if(fd < 0){
        perror("SHM: failed to open");
    }
    ftruncate(fd,length);

    segment = (char*) mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(!segment){
        perror("SHM: failed to map");
    }

    recv_done = sem_open((name+"recv").c_str(), O_CREAT, 0600, 1);
    send_done = sem_open((name+"send").c_str(), O_CREAT, 0600, 0);
}

SHMQueue::~SHMQueue(){
    sem_close(send_done);
    sem_close(recv_done);

    sem_unlink((name+"recv").c_str());
    sem_unlink((name+"send").c_str());

    munmap(segment, length);

    shm_unlink(name.c_str());
}

int SHMQueue::shm_receive (void* msgbuf, int msgsize){
    sem_wait(send_done);

    memcpy(msgbuf, segment, msgsize);

    sem_post(recv_done);

    return msgsize;
}

int SHMQueue::shm_send (void* msgbuf, int msgsize){
    sem_wait(recv_done);

    memcpy(segment, msgbuf, msgsize);

    sem_post(send_done);

    return msgsize;
}

SHMRequestChannel::SHMRequestChannel (const string _name, const Side _side, int _length){
    my_name = _name;

    string s1 = "/SHM"+_name+"1";
    string s2 = "/SHM"+_name+"2";

    shmQueue1 = new SHMQueue(s1.c_str(), _length);
    shmQueue2 = new SHMQueue(s2.c_str(), _length);

    if(_side == SENDING_SIDE){
        swap(shmQueue1, shmQueue2);
    }
}

SHMRequestChannel::~SHMRequestChannel () {
    delete shmQueue1;
    delete shmQueue2;
}

int SHMRequestChannel::cread (void* msgbuf, int bufcapacity) {
    return shmQueue1-> shm_receive(msgbuf, bufcapacity);
}

int SHMRequestChannel::cwrite (void* msgbuf, int msglen) {
    return shmQueue2 -> shm_send(msgbuf, msglen);
}

string SHMRequestChannel::getName(){
    return my_name;
}