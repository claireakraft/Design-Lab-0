#include "mbed.h"
#include "CKraft_binaryutils.h"
//#include <thread>

#define set (uint32_t *)0x50000508
#define clear (uint32_t *)0x5000050C
#define dir (uint32_t *)0x50000514
#define pin (uint8_t)13


// main() runs in its own thread in the OS
int main()
{

    setbit(dir, pin);
    

    while (true) {
        setbit(set, pin);
        thread_sleep_for(1000);
        setbit(clear, pin);
        thread_sleep_for(1000);

    }
}

