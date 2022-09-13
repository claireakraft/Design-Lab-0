#include "CKraft_binaryutils.h"
#include "mbed.h"
#include "USBSerial.h"

#define set (uint32_t *)0x50000508
#define clear (uint32_t *)0x5000050C
#define dir (uint32_t *)0x50000514
#define pin (uint8_t)13



typedef struct {
    int speed;
} mail_t;

enum states {
    NO_ERROR = 1200,
    ATTN_REQ = 800,
    FATAL_ERROR = 400
};

Mail<mail_t, 16> mail_box;

Thread thread1;
Thread thread2;
USBSerial serial;

void led_diag_handler() {

    int state = NO_ERROR;

    while (true) {
        if (!mail_box.empty()) {
            
            mail_t *mail = mail_box.try_get();
            state = mail->speed;
            mail_box.free(mail);

            if(state == NO_ERROR){
                serial.printf("State change to NO_ERROR at 0.416 Hz\r\n");
            }
            else if(state == ATTN_REQ){
                serial.printf("State change to ATTN_REG at 0.625 Hz\r\n");
            }
            else if(state == FATAL_ERROR){
                serial.printf("State change to FATAL_ERROR at 1.25 Hz\r\n");
            }

        }

        setbit(set, pin);
        thread_sleep_for(state);
        setbit(clear, pin);
        thread_sleep_for(state);
    }
}

void diag_tester() {
    int state = 0;
    while (true) {
        mail_t *mail = mail_box.try_alloc();

        if (state == 0) {
            state = 1;
            mail->speed = NO_ERROR;

        } else if (state == 1) {
            state = 2;
            mail->speed = ATTN_REQ;
        } else if (state == 2) {
            state = 0;
            mail->speed = FATAL_ERROR;
        }

        mail_box.put(mail);
        thread_sleep_for(5000);
    }
}

// main() runs in its own thread in the OS
int main() {

    setbit(dir, pin);
    
    thread1.start(led_diag_handler);
    thread2.start(diag_tester);
    serial.printf("Threads initialized\r\n");

    while (true) {
        serial.printf("Hello from main!\r\n");
        thread_sleep_for(5000);

        // setbit(set, pin);
        // thread_sleep_for(1000);
        // setbit(clear, pin);
        // thread_sleep_for(1000);
    }
}
