#ifndef PAQUETE_H__
#define PAQUETE_H__
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <inttypes.h>

typedef struct paquete{

    union
    {
      char *init_pkt;
      uint16_t header;
    };
    uint16_t len;
     union{
        char buff[8];
        struct{
            uint32_t dato1;
            uint32_t dato2;    
        };
    };
    uint32_t trailer;
    uint32_t crc;
}paquete_t;

uint32_t crc32_le(paquete_t *pkt);

#endif