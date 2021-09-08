#include "posix_shm.h"
#include <iostream>
#include <errno.h>
#include <string.h>

SharedMemory shm_probe;

SharedMemory::SharedMemory (std::string name, int size) {
    map_size = size;
    int fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
    if (ftruncate(fd, size) < 0) {
        std::cerr << "ftruncate error: " << errno << std::endl;
        exit(1);
    }

    data = (SHMD *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED) {
        std::cerr << "fail to map memory\n";
        exit(1);
    }

#if !(defined(ATTACK_SIGNAL_GENERATOR) && defined(LOG_DATA))
    init();
#endif
}

SharedMemory::~SharedMemory () {
    munmap(data, map_size);
}

void SharedMemory::init () {
    memset(data, 0, sizeof(SHMD));
    setInjectMode(SHMMode::NonAttack);
}

enum SHMMode SharedMemory::getInjectMode () {
    return data->inject_data.flag;
}

void SharedMemory::setInjectMode (enum SHMMode mode) {
    data->inject_data.flag = mode;
}

InjectData *SharedMemory::getInjectData() {
    return &data->inject_data;
}

void SharedMemory::leakCommit () {
    data->leak_data.time = clock();
}

LeakData *SharedMemory::getLeakData () {
    return &data->leak_data;
}

#ifdef ATTACK_SIGNAL_GENERATOR
#include <time.h>
#include <stdlib.h>
#define MAX_VALUE 1000 // 1.000 ~ 1.000
int main () {
    srand(time(NULL));
    shm_probe.setInjectMode(SHMMode::StartAttack);
    while (1) {
        InjectData *data = shm_probe.getInjectData();
        data->roll_rate = (float)(rand() % (MAX_VALUE * 2) - MAX_VALUE) / MAX_VALUE; // -1 ~ 1
        data->yaw_rate = (float)(rand() % (MAX_VALUE * 2) - MAX_VALUE) / MAX_VALUE; // -1 ~ 1
        data->pitch_rate = (float)(rand() % (MAX_VALUE * 2) - MAX_VALUE) / MAX_VALUE; // -1 ~ 1
        data->throttle_rate = (float)(rand() % MAX_VALUE) / MAX_VALUE; // 0 ~ 1
    }
}

#endif

#ifdef LOG_DATA
using namespace std;
int main () {
    time_t pre_time = 0;
    LeakData *data = shm_probe.getLeakData();
    while (1) {
        if (data->time != pre_time) {
            pre_time = data->time;
            cout << (double)data->time / CLOCKS_PER_SEC << ": " << data->pid_piper[0] << ", " << data->pid_piper[1] << ", " << data->pid_piper[2] << "\n";
        }
    }
}
#endif
