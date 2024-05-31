#ifndef BME280C_TYPES_H__
#define BME280C_TYPES_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BME280_DIR_SLAVE (0xF6)
#define BME280_DIR_ID (0xD0)        //Direccion del Identificador
#define BME280_DIR_CRT_H (0xF2)     //Direccion reg control humedad
#define BME280_DIR_CRT_M (0xF4)     //Direccion reg meas
#define BME280_DIR_CONF (0xF5)      //Direccion reg configuracion
#define BME280_DIR_STATUS (0xF3)    //Direccion reg status
#define BME280_DIR_RESET (0xE0)    //Direccion reg reset
#define BME280_RESET_VALUE (0xB6)

#define PRESS_MSB (0xF7)        //Direccion reg value press
#define PRESS_LSB (0xF8)        //Direccion reg value press
#define PRESS_XLS (0xF9)        //Direccion reg value press

#define TEMP_MSB (0xFA)         //Direccion reg value TEMP
#define TEMP_LSB (0xFB)         //Direccion reg value TEMP
#define TEMP_XLS (0xFC)         //Direccion reg value TEMP

#define HUM_MSB (0xFD)          //Direccion reg value Humedad
#define HUM_LSB (0xFE)          //Direccion reg value Humedad

#ifndef CALIB_REGS
#define CALIB_REGS

#define dig_T1_MSB (0x89)
#define dig_T1_LSB (0x88)
#define dig_T2_MSB (0x8B)
#define dig_T2_LSB (0x8A)
#define dig_T3_MSB (0x8D)
#define dig_T3_LSB (0x8C)

#define dig_P1_MSB (0x8F)
#define dig_P1_LSB (0x8E)
#define dig_P2_MSB (0x91)
#define dig_P2_LSB (0x90)
#define dig_P3_MSB (0x93)
#define dig_P3_LSB (0x92)
#define dig_P4_MSB (0x95)
#define dig_P4_LSB (0x94)
#define dig_P5_MSB (0x97)
#define dig_P5_LSB (0x96)
#define dig_P6_MSB (0x99)
#define dig_P6_LSB (0x98)
#define dig_P7_MSB (0x9B)
#define dig_P7_LSB (0x9A)
#define dig_P8_MSB (0x9D)
#define dig_P8_LSB (0x9C)
#define dig_P9_MSB (0x9F)
#define dig_P9_LSB (0x9E)

#define dig_H1     (0xA1)
#define dig_H2_MSB (0xE2)
#define dig_H2_LSB (0xE1)
#define dig_H3     (0xE3)
#define dig_H4_MSB (0xE4)
#define dig_H4_LSB (0xE5)

#define dig_H5_MSB (0xE6)
#define dig_H5_LSB (0xE5)
#define dig_H6     (0xE7)
#endif

extern const char *On;
extern const char *Off;

typedef struct calib_Temp_tag
{
    uint16_t values[3];
} calib_Temp_t;

typedef struct calib_Press_tag
{
    uint16_t values[9];
}calib_Press_t;

typedef struct calib_Hum_tag
{
    uint8_t valuesu8[3];
    uint16_t valuesu16[3];
} calib_Hum_t;

typedef struct BME280_tag
{
    uint8_t whoami;
    int32_t t_fine;
    float temp;
    float humedad;
    //float press;
    bool state;
    TaskHandle_t task;
} BME280_t;

typedef struct Utils_tag
{
    BME280_t BME280;
    bool ledState;
    const char *ledState_char;
}Utils_t;

#endif