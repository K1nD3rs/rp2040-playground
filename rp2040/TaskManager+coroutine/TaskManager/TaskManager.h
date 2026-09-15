#pragma once
#include "coroutine.h"
#include "stdint.h"
#include <cstdint>
#include <hardware/timer.h>
#include <hardware/uart.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdio.h>

#define MS * 1000
#define SEC * 1000000



#ifndef MAX_TASKS
#define MAX_TASKS 16
#endif

using TaskFn = void(*)(); // alias 
using TaskCoroutineFn = void(*)(Coroutine*); // alias 


enum class TaskType {
    Normal,
    Coroutine
};

    
typedef struct {
    TaskType type;

    TaskFn fn;
    TaskCoroutineFn coroutine_fn;

    Coroutine* coroutine;


    uint64_t period;
    uint64_t last;
    bool active;
} Task;


class TaskManager {
public:

    bool add(TaskFn fn, uint32_t period_ms) {
        Task task;

        task.fn = fn;
        task.type = TaskType::Normal;
        task.period = period_ms;
        task.last = 0;
        task.active = 1;

        task.coroutine = nullptr;
        task.coroutine_fn = nullptr;

        if (tasks_index < MAX_TASKS) {
            this->tasks[tasks_index] = task;
            tasks_index++; 
            if(stdio_usb_connected()) {printf("task%d+  ", tasks_index);}
            return 1;
        } else {
            if(stdio_usb_connected()) {printf("task%d-  ", tasks_index);}
            return 0;
        }
    }

bool add(TaskCoroutineFn fn, Coroutine& co) {
        Task task;

        task.coroutine_fn = fn;
        task.coroutine = &co;
        task.type = TaskType::Coroutine;

        task.last = 0;
        task.active = 1;

        task.fn = nullptr;

        if (tasks_index < MAX_TASKS) {
            this->tasks[tasks_index] = task;
            tasks_index++; 
            if(stdio_usb_connected()) {printf("task%d+  ", tasks_index);}
            return 1;
        } else {
            if(stdio_usb_connected()) {printf("task%d-  ", tasks_index);}
            return 0;
        }
    }

    void tick() {
            uint64_t now = time_us_64(); // ! два варианта. в цикле\вне цикла
            for(int i = 0; i < tasks_index; i++){
            
                Task& task = tasks[i];
                
                if(task.type == TaskType::Normal){
                    if(now - task.last >= task.period){
                        task.fn();
                        task.last = now;
                    }
                }

                else if(task.type == TaskType::Coroutine){

                    if(task.coroutine->state == -1) continue;
                    if(now >= task.coroutine->wake_time){
                        task.coroutine_fn(task.coroutine);

                    }
                }
        };
    }

    int min_time = INT32_MAX;
    int tasks_index = 0;
    Task tasks[MAX_TASKS];
    

};


/*             if( now - this->tasks[i].last >= this->tasks[i].period) {

                if (this->tasks[i].type == TaskType::Normal) this->tasks[i].fn();
                    else
                if (this->tasks[i].type == TaskType::Coroutine) this->tasks[i].coroutine_fn(this->tasks[i].coroutine);

                this->tasks[i].last = now;
            } */