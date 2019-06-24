#include "mbed.h"
#include "C12832.h"

InterruptIn sw2(SW2);
DigitalOut led(LED1);
C12832 lcd(D11, D13, D12, D7, D10);
Ticker counter; // https://os.mbed.com/handbook/Ticker
PwmOut pwm(D6);

int count = 0; // counter for displaying
int play_tone_flag = 0;
int display_counter_flag = 0;

EventQueue queue;

void blink_led(void) {
    led = !led;
    wait(0.5);
    led = !led;
}

void display_counter(void) {
    lcd.cls();
    lcd.locate(0,0);
    lcd.printf("%d", count);
    if((count % 10 == 0) && (count != 0)) {
        blink_led();          
    }
    count++;
}

void display_counter_interrupt(void) {
    queue.call(&display_counter);
}

void play_tone_interrupt(void) {
    play_tone_flag = 1;
}

void play_tone(void) {
    // https://os.mbed.com/handbook/PwmOut
    // https://os.mbed.com/users/4180_1/notebook/using-a-speaker-for-audio-output/
    for(int i = 0; i < 4000 ; i += 200) {
        pwm.period(1.0/float(i));
        pwm = 0.25;
        wait(.1);
    }
    pwm = 0.0;
}

int main(void) {
    // An extra thread to off load printf to when the ticker interrupt occurs
    Thread eventThread;
    eventThread.start(callback(&queue, &EventQueue::dispatch_forever));
    
    led = !led; // The led on my board would always start on 
    
    counter.attach(queue.event(&display_counter_interrupt), 1.0f);
    sw2.fall(&play_tone_interrupt);
    
    while(1) {
        if(play_tone_flag) {
            play_tone_flag = 0;
            play_tone();
        }
        sleep();
    }
    
}