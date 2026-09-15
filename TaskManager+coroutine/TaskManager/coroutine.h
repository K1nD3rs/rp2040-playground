#pragma once

#include <stdint.h>

class Coroutine {
public:
    int state = 0;
    uint64_t wake_time = 0;
};



// ! Точка входа
#define co_begin \
    switch ((co)->state) { \
        case 0:
    
// ! вернуть управление, продолжить позже с этого места. 
#define co_yield \
    (co)->wake_time = 0;    \
    (co)->state = __LINE__; \
    return; \
    case __LINE__:

// ! co_end — завершение корутины, сброс состояния в 0
#define co_end \
    (co)->state = 0; \
    break; \
    } \
    return;
// ? ????

/* // ! co_end — завершение корутины, сброс состояния в 0
#define co_end \
    } \
    (co)->state = 0; */

//! co_return — окончательно завершить корутину (state = -1)
#define co_return \
    (co)->state = -1; \
    return;

//! co_await_ms — ждать ms миллисекунд, не блокируя поток
#define co_await_ms(ms)          \
    (co)->wake_time = time_us_64() + \
                      (uint64_t)(ms) * 1000ULL; \
    (co)->state = __LINE__;          \
    return;                          \
    case __LINE__:
    
//! co_await_us — ждать микросекунды
#define co_await_us(us) \
    (co)->wake_time = time_us_64() + (uint64_t)(us); \
    (co)->state = __LINE__; \
    return; \
    case __LINE__:

//! co_wait_until — ждать до абсолютного времени
#define co_wait_until(t) \
    (co)->wake_time = (t); \
    (co)->state = __LINE__; \
    return; \
    case __LINE__:


//! ai slop
#define co_await_until(cond) \
    while (!(cond)) { \
        (co)->state = __LINE__; \
        return; \
        case __LINE__:; \
    }

//? need to add co_await_until_or_timeout