#include "TaskManager/coroutine.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <hardware/gpio.h>
#include "pico/stdio_usb.h"
#include <hardware/structs/i2c.h>
#include <hardware/timer.h>
#include <hardware/uart.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdint.h>
#include "hardware/clocks.h"

#define MAX_TASKS 11

#include "TaskManager/TaskManager.h"





TaskManager manager;

Coroutine co;
Coroutine co_butt;



void task1() {
    gpio_put(25, !gpio_get(25));
}

void task2()
{
    static uint64_t ticks = 0;

    ticks++;

    printf("heartbeat;  minutes: %llu\n", ticks);
    
}


void coro_button(Coroutine* co)
{
    static int stable_state   = 1;
    static int last_state     = 1;
    static int stable_counter = 0;
    static int raw = 0;

    co_begin;

    while (true) {
        raw = gpio_get(24);

        if (raw != last_state) {
            last_state = raw;
            stable_counter = 0;
        } else {
            stable_counter++;
        }

        if (stable_counter >= 5 && stable_state != last_state) {
            stable_state = last_state;
            if (stable_state == 0) {
                printf("[button] PRESSED\n");
            } else {
                printf("[button] RELEASED\n");
            }
        }

        co_await_ms(10);
    }

    co_end;
}


void task(Coroutine* co)
{

    static uint64_t counter = 0;
    static uint64_t i = 0;

    co_begin;

    while(1){
        for(i = 0; i < 10; i++){
            counter += i;
            co_yield;
        }
        co_await_ms(1000);
        //printf("%f\n", (double)counter);
        counter = 0;
    }
    
    co_end;
}






int main() {
    stdio_init_all();
    
    set_sys_clock_khz(200000, true);

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    gpio_init(24);
    gpio_set_dir(24, GPIO_IN);
    gpio_pull_up(24);


    //while (!stdio_usb_connected());

    sleep_ms(500);
    
    manager.add(task1, 250 MS);   // 100 мс
    manager.add(task2, 60 SEC);   // 250 мс
    //manager.add(print_iterations, 1000 MS); // 1 сек
    manager.add(task, co);
    manager.add(coro_button, co_butt);
    //manager.add(task, 1000 * 1000); // 1 сек

    while(true) {

    
        manager.tick();
        
        //iterations++;
    }





}