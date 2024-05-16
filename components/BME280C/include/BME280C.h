#ifndef BBME280_h
#define BBME280_h

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define I2C_MASTER_SCL_IO           (19)
#define I2C_MASTER_SDA_IO           (18)
#define I2C_MASTER_NUM              (0)
#define I2C_MASTER_FREQ_HZ          (400000)
#define I2C_MASTER_TX_BUF_DISABLE   (0)
#define I2C_MASTER_RX_BUF_DISABLE   (0)
#define I2C_MASTER_TIMEOUT_MS       1000

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

typedef struct sensor_values_tag
{
    uint8_t whoami;
    int32_t t_fine;
    float temp;
    float humedad;
    float press;
    bool state;
    TaskHandle_t task;
} sensor_value_t;

const static uint8_t dir_CTemp[] = {dig_T1_LSB, dig_T2_LSB, dig_T3_LSB};

const static uint8_t dir_CPress[] = {dig_P1_LSB, dig_P2_LSB, dig_P3_LSB, dig_P4_LSB, dig_P5_LSB, dig_P6_LSB, dig_P7_LSB, dig_P8_LSB, dig_P9_LSB};

#define LED GPIO_NUM_2

void init_BME2890(void);

void calib_values(void);

void BME280_read(uint8_t reg_addr, uint8_t *data, size_t len);

void BME280_write_byte(uint8_t reg_addr, uint8_t data);

uint32_t read_Press();

uint32_t read_Temp();

uint32_t read_Hum();

void calib_Press(sensor_value_t *sens);

void calib_Temp(sensor_value_t *sens);

void calib_Hum(sensor_value_t *sens);

void init_led();

void set_led(bool state);

#endif