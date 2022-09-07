#include "CKraft_binaryutils.h"
#include "mbed.h"
//#include <thread>

#define set (uint32_t *)0x50000508
#define clear (uint32_t *)0x5000050C
#define dir (uint32_t *)0x50000514
#define pin (uint8_t)13

Thread thread;

void thread() {
    while (true) {
    }
}

// main() runs in its own thread in the OS
int main() {

    thread.start(thread);

    // setbit(dir, pin);

    while (true) {

        ThisThread::sleep_for(5000);

        // setbit(set, pin);
        // thread_sleep_for(1000);
        // setbit(clear, pin);
        // thread_sleep_for(1000);
    }
}
