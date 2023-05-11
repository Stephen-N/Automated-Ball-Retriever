#include <unistd.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
using namespace std;

int main(int argc, char const *argv[]) {
    ofstream myfile;
    for (size_t i = 0; i < 11; i++) {
        myfile.open("sensor_info.txt", std::ofstream::out | std::ofstream::trunc);
        myfile << i;
        if (i == 10)
            i = 0;
        myfile.close();
        cout << i << endl;
        // sleep(1);
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    return 0;
}
