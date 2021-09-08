#ifndef POSIX_SHM_H
#define POSIX_SHM_H
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <time.h>

enum SHMMode {
    NonAttack,
    StartAttack,
};

typedef struct inject_data {
    enum SHMMode flag;
    float roll_rate, yaw_rate, pitch_rate, throttle_rate;
} InjectData;

// TODO: leak information by shared memory
typedef struct leak_data {
    clock_t time;
    float pid_piper[3];
} LeakData;

typedef struct shared_memory_data {
    InjectData inject_data __attribute__((aligned(64)));
    LeakData leak_data __attribute__((aligned(64)));
    //InjectData inject_data;
    //LeakData leak_data;
} SHMD;

class SharedMemory {
public:

    SharedMemory (std::string name = "ardupilot_shm", int size = 0x40000);
    ~SharedMemory ();
    void init ();

    enum SHMMode getInjectMode ();
    void setInjectMode (enum SHMMode mode);
    InjectData *getInjectData ();

    void leakCommit ();
    LeakData *getLeakData ();

private:
    SHMD *data;
    int map_size;
};

extern SharedMemory shm_probe;

#endif
