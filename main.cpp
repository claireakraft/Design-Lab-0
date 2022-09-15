/*
Author : Claire Kraft
Date Modified : Sept. 9 2022
What does this program do : this program uses threads and mail boxes 
to communicate with each other to change the blinking speed of a LED every 
5 seconds. 
*/

#include "CKraft_binaryutils.h"
#include "USBSerial.h"
#include "mbed.h"

#define set (uint32_t *)0x50000508
#define clear (uint32_t *)0x5000050C
#define dir (uint32_t *)0x50000514
#define pin (uint8_t)13

// this is the thing that the mail is sending
typedef struct {
    int speed;
} mail_t;

// create a variable type for the different classes
enum states {
    NO_ERROR = 1200,
    ATTN_REQ = 800,
    FATAL_ERROR = 400
};

Mail<mail_t, 16> mail_box;

// initilaize threads and serial
Thread thread1;
Thread thread2;
USBSerial serial;

// this thread receives messages from other thread and changes the blinking speeds
void led_diag_handler() {
    // set the first state as no error
    int state = NO_ERROR;

    while (true) {
        // when mail box is not empty do this
        if (!mail_box.empty()) {

            //get the mail from the mail box
            mail_t *mail = mail_box.try_get();
            state = mail->speed;
            // free the mail so you don't run out of storage
            mail_box.free(mail);

            // if else statement to print to serial when the blinking state changes
            if (state == NO_ERROR) {
                serial.printf("State change to NO_ERROR at 0.416 Hz\r\n");
            } else if (state == ATTN_REQ) {
                serial.printf("State change to ATTN_REG at 0.625 Hz\r\n");
            } else if (state == FATAL_ERROR) {
                serial.printf("State change to FATAL_ERROR at 1.25 Hz\r\n");
            }
        }

        // these lines turn on and off LED and allow blinking
        setbit(set, pin);
        thread_sleep_for(state);
        setbit(clear, pin);
        thread_sleep_for(state);
    }
}

// this thread sends messages to other thread when to change the blinking speed and what to change it to
void diag_tester() {

    int state = 0;
    while (true) {
        // alloc the mail going to send
        mail_t *mail = mail_box.try_alloc();
        // if statement to keep track of the current speed state and what to change it to
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
        // sending the mai to the mail box
        mail_box.put(mail);
        // send the mail every 5 seconds
        thread_sleep_for(5000);
    }
}

// main() runs in its own thread in the OS
int main() {

    setbit(dir, pin);

    // starting the threads
    thread1.start(led_diag_handler);
    thread2.start(diag_tester);
    serial.printf("Threads initialized\r\n");

    while (true) {
        serial.printf("Hello from main!\r\n");
        // put main thread to sleep so other threads can run
        thread_sleep_for(5000);

        // code left from part one and two of the lab
        // setbit(set, pin);
        // thread_sleep_for(1000);
        // setbit(clear, pin);
        // thread_sleep_for(1000);
    }
}
