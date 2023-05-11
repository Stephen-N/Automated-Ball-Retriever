#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>

#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <iostream>  //including libraries
#include <pqxx/pqxx>
#include <string>
#include <thread>

#include "SHMRequestChannel.h"
#include "common.h"

using namespace std;
using namespace pqxx;

const int IN1 = 23;  // variable for pinmode logic control
const int IN2 = 4;
const int IN3 = 12;
const int IN4 = 25;

// WHEN SWITCHING FWD->REVERSE BRAKE > 0.1S
//***CANNOT REVERSE WHEN MOTOR IS NOT STOPPED

/**
 * 12 left (forward,back)
 * 34 right (forward,back)
 */

void brake() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void reverse_speed() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}

void forward_to_speed() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void turnLeft() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}

void writeGUI(string filename) {
    ofstream myfile(filename);
    if (myfile.is_open()) {
        printf("Writing to %s...\n", filename.c_str());
    } else {
        printf("%s could not open!\n", filename.c_str());
    }
    /* Send information to GUI */

    myfile.close();
}

void controlMotors(string gui_info) {
    /* Process information from GUI */
    stringstream ss(gui_info);
    string s;
    vector<string> out;
    float robotVelocity = 0;
    int robotOmega = 0;

    while (getline(ss, s, ' ')) {
        out.push_back(s);
    }
    if (out.size() == 2) {
        robotVelocity = stof(out[0]);
        robotOmega = stof(out[1]);
    }
    printf("robotVelocity = %.6f\trobotOmega = %d\n", robotVelocity, robotOmega);

    /* Control Motors */
    wiringPiSetupGpio();  // set up gpio

    pinMode(IN1, OUTPUT);  // pinmode for IN pins
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    if (robotVelocity == 0 && robotOmega == 0) {
        brake();
        if (robotOmega > 0) {
            turnRight();
        } else if (robotOmega < 0) {
            turnLeft();
        }
    } else if (robotVelocity < 0) {
        forward_to_speed();
    } else if (robotVelocity > 0) {
        reverse_speed();
    }
}

void readGUI(string filename) {
    cout << filename << endl;
    ifstream myfile(filename);
    string gui_info = "123";
    if (myfile.is_open()) {
        getline(myfile, gui_info);
        // printf("Current gui input: %s\n", gui_info.c_str());
    } else {
        printf("File could not open!\n");
    }
    controlMotors(gui_info);
    myfile.close();
}

void PopulateDB(connection &C, float distance, double ultrasonic[], bool magnet[], bool photo[], int num_ultrasonic, int num_photo) {
    string str_distance = to_string(distance);
    string str_ultrasonic[num_ultrasonic] = {};
    for (int i = 0; i < num_ultrasonic; i++) {
        str_ultrasonic[i] = to_string(ultrasonic[i]);
    }

    string str_photo[num_photo] = {};
    str_photo[0] = to_string(photo[0]);
    str_photo[1] = to_string(photo[1]);

    string insert_query = "INSERT INTO statistics(distance, us1, us2, us3, us4, us5, photo1, photo2) ";
    string values_query = "VALUES (" + str_distance + "," +
                          str_ultrasonic[0] + "," +
                          str_ultrasonic[1] + "," +
                          str_ultrasonic[2] + "," +
                          str_ultrasonic[3] + "," +
                          str_ultrasonic[4] + "," +
                          str_photo[0] + "," +
                          str_photo[1] +
                          ");";
    string test = insert_query + values_query;
    char *sql = const_cast<char *>(test.c_str());

    /* Create a transactional object. */
    work W(C);

    /* Execute SQL query */
    W.exec(sql);
    W.commit();
    cout << "Records created successfully" << endl;
}

float randomDouble(double a, double b) {
    double random = ((double)rand()) / (double)RAND_MAX;
    double diff = b - a;
    double r = random * diff;
    return a + r;
}

void UpdateData(SHMRequestChannel *chan, float &distance, double *ultrasonic, bool *magnet, bool *photo, int num_ultrasonic, int num_photo) {
    int sensor_id = 0;

    // updating ultrasonic sensor values
    // char *ultraBuf = new char[sizeof(ULTRASONICDATA_MSG)];
    // for (int i = 0; i < num_ultrasonic; i++) {
    //     chan->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    //     ultraSonicData *m_ultra = (ultraSonicData *)ultraBuf;
    //     sensor_id = m_ultra->sensor;
    //     double value = m_ultra->value;
    //     ultrasonic[sensor_id] = value;
    // }

    // char *ultraBuf = new char[sizeof(ULTRASONICDATA_MSG)];
    // SHMRequestChannel *ultraChan = process_newReciving_request("1", ultraBuf);
    // ultraChan->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    // ultraSonicData *m_ultra = (ultraSonicData *)ultraBuf;
    // sensor_id = m_ultra->sensor;
    // value = m_ultra->value;
    // ultrasonic[0] = value;

    // SHMRequestChannel *ultraChan2 = process_newReciving_request("2", ultraBuf);
    // ultraChan2->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    // ultraSonicData *m_ultra2 = (ultraSonicData *)ultraBuf;
    // sensor_id = m_ultra2->sensor;
    // value = m_ultra2->value;
    // ultrasonic[1] = value;

    // SHMRequestChannel *ultraChan3 = process_newReciving_request("3", ultraBuf);
    // ultraChan3->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    // ultraSonicData *m_ultra3 = (ultraSonicData *)ultraBuf;
    // sensor_id = m_ultra3->sensor;
    // value = m_ultra3->value;
    // ultrasonic[2] = value;

    // SHMRequestChannel *ultraChan4 = process_newReciving_request("4", ultraBuf);
    // ultraChan4->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    // ultraSonicData *m_ultra4 = (ultraSonicData *)ultraBuf;
    // sensor_id = m_ultra4->sensor;
    // value = m_ultra4->value;
    // ultrasonic[3] = value;

    // SHMRequestChannel *ultraChan5 = process_newReciving_request("5", ultraBuf);
    // ultraChan4->cread(ultraBuf, sizeof(ULTRASONICDATA_MSG));
    // ultraSonicData *m_ultra5 = (ultraSonicData *)ultraBuf;
    // sensor_id = m_ultra5->sensor;
    // value = m_ultra5->value;
    // ultrasonic[4] = value;

    // // updating magnet sensor values
    // char *magBuf = new char[sizeof(MagData_MSG)];
    // chan->cread(magBuf, sizeof(MagData_MSG));
    // magData *m_magnet = (magData *)magBuf;
    // double magnet1, magnet2 = -1;
    // magnet1 = m_magnet->value;
    // magnet2 = m_magnet->value2;
    // magnet[0] = magnet1;
    // magnet[1] = magnet2;

    // // updating photo sensor values
    // char *photoBuf = new char[sizeof(PHOTO_MSG)];
    // for (int i = 0; i < num_photo; i++) {
    //     chan->cread(photoBuf, sizeof(PHOTO_MSG));
    //     photoData *m_photo = (photoData *)photoBuf;
    //     sensor_id = m_photo->sensor;
    //     bool alarm_val = m_photo->alarm;
    //     photo[sensor_id] = alarm_val;
    // }

    // placeholder values, to be deleted
    distance += randomDouble(1, 10);
    ultrasonic[0] = randomDouble(1, 10);
    ultrasonic[1] = randomDouble(1, 10);
    ultrasonic[2] = randomDouble(1, 10);
    ultrasonic[3] = randomDouble(1, 10);
    ultrasonic[4] = randomDouble(1, 10);
    photo[0] = rand() % 2;
    photo[1] = rand() % 2;

    string write_path = "site/app/sensor_info.txt";
    ofstream myfile(write_path);
    if (myfile.is_open()) {
        // printf("Writing to %s...\n", write_path.c_str());
    } else {
        printf("%s could not open!\n", write_path.c_str());
    }
    /* Send information to GUI */
    myfile << ultrasonic[0] << endl;
    myfile << ultrasonic[1] << endl;
    myfile << ultrasonic[2] << endl;
    myfile << ultrasonic[3] << endl;
    myfile << ultrasonic[4] << endl;
    myfile << photo[0] << endl;
    myfile << photo[1] << endl;

    myfile.close();
}

void signalHandler(int signum) {
    cout << "Interrupt signal (" << signum << ") received.\n";

    // cleanup and close up stuff here & terminate program
    exit(signum);
}

int main(int argc, char *argv[]) {
    int num_ultrasonic = 5;
    int num_photo = 2;
    double ultrasonic[num_ultrasonic] = {-1, -1, -1, -1, -1};
    double *ptr_ultrasonic = ultrasonic;
    float distance = 0;
    bool moving = false;
    float ir = 0;
    bool magnet[2] = {false, false};  // a single magnet sensor returns 2 values
    bool *ptr_magnet = magnet;
    bool photo[num_photo] = {false, false};
    bool *ptr_photo = photo;
    string read_path = "site/app/gui_info.txt";
    string write_path = "site/app/sensor_info.txt";

    SHMRequestChannel *chan = new SHMRequestChannel("reader", RECIVING_SIDE, MAX_MESSAGE);

    signal(SIGINT, signalHandler);

    /* Connect to db, get sensor values, & populate db */
    try {
        connection C(
            "dbname = capstone \
         user = postgres \
         hostaddr = 127.0.0.1 \
         port = 5432 \
         password = password");
        if (C.is_open()) {
            cout << "Opened database successfully: " << C.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return 1;
        }

        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            UpdateData(chan, distance, ptr_ultrasonic, ptr_magnet, ptr_photo, num_ultrasonic, num_photo);
            PopulateDB(C, distance, ultrasonic, magnet, photo, num_ultrasonic, num_photo);
            readGUI(read_path);
        }
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
}