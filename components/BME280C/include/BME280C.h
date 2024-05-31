#ifndef BBME280_h
#define BBME280_h

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "Config.h"
#include "BME280C_types.h"
#include "Wifi.h"

#define I2C_MASTER_SCL_IO           (19)
#define I2C_MASTER_SDA_IO           (18)
#define I2C_MASTER_NUM              (0)
#define I2C_MASTER_FREQ_HZ          (400000)
#define I2C_MASTER_TX_BUF_DISABLE   (0)
#define I2C_MASTER_RX_BUF_DISABLE   (0)
#define I2C_MASTER_TIMEOUT_MS       1000


#if(MODOESP == SLAVE1)
const static uint8_t dir_CTemp[] = {dig_T1_LSB, dig_T2_LSB, dig_T3_LSB};

//const static uint8_t dir_CPress[] = {dig_P1_LSB, dig_P2_LSB, dig_P3_LSB, dig_P4_LSB, dig_P5_LSB, dig_P6_LSB, dig_P7_LSB, dig_P8_LSB, dig_P9_LSB};

#define LED GPIO_NUM_2

void init_slave1(Utils_t *utils, slave_state_t *slave);

void init_BME2890(void);

void calib_values(void);

void BME280_read(uint8_t reg_addr, uint8_t *data, size_t len);

void BME280_write_byte(uint8_t reg_addr, uint8_t data);

//uint32_t read_Press();

uint32_t read_Temp();

uint32_t read_Hum();

//void calib_Press(BME280_t *sens);

void calib_Temp(BME280_t *sens);

void calib_Hum(BME280_t *sens);

void init_led();

void set_led(bool state);
#endif

#endif