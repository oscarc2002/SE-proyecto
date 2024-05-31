#ifndef _CONFIG_H__
#define _CONFIG_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAESTRO (1)
#define SLAVE1 (2)
#define SLAVE2 (3)

//#define MODOESP MAESTRO
#define MODOESP (SLAVE1)

typedef struct slave_state{
    bool connect_wf;
    TaskHandle_t sender;
    void *arg;
} slave_state_t;

#endif