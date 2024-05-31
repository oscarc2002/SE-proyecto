#include <stdio.h>
#include "BME280C.h"
#include "sdkconfig.h"

const char *On = "Encendido";
const char *Off = "Apagado";

#if (MODOESP == SLAVE1)
static calib_Temp_t ATemp;
//static calib_Press_t APress;
static calib_Hum_t AHum;

static void read_BME280(void *arg);

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

}

static esp_err_t BME280_reg_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, BME280_DIR_SLAVE, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static esp_err_t BME280_reg_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};

    ret = i2c_master_write_to_device(I2C_MASTER_NUM, BME280_DIR_SLAVE, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);

    return ret;
}

void init_BME2890(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_ERROR_CHECK(BME280_reg_write_byte(BME280_DIR_RESET, BME280_RESET_VALUE));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(BME280_reg_write_byte(BME280_DIR_CRT_H, (uint8_t)(1 << 2 | 1)));
    ESP_ERROR_CHECK(BME280_reg_write_byte(BME280_DIR_CRT_M, (uint8_t) (1 << 7 | 1 << 5 | 1 << 4 | 1 << 2 | 1)));
    ESP_ERROR_CHECK(BME280_reg_write_byte(BME280_DIR_CONF, (uint8_t) (1 << 4)));
    calib_values();
    return;
}

void init_slave1(Utils_t *utils, slave_state_t *slave)
{
    ESP_ERROR_CHECK(init_wifi());
    init_BME2890();
    init_led();
    slave->arg = utils;
    slave->connect_wf = false;
    utils->ledState_char = Off;
    utils->ledState = false;
    utils->BME280.state = true;
    xTaskCreate(read_BME280, "read_BME280", 1024*16, &utils->BME280, 5, (TaskHandle_t *const) &utils->BME280.task);
}

void calib_values(void)
{
    
    uint8_t buff[2], i;

    for (i = 0; i < 3; i++)
    {
        BME280_reg_read(dir_CTemp[i], &buff[0], 1);
        BME280_reg_read((dir_CTemp[i])+1, &buff[1], 1);
        ATemp.values[i] =(uint16_t) ((buff[1] << 8) | buff[0]);

    }
    /*
    for (i = 0; i < 9; i++)
    {
        BME280_reg_read(dir_CPress[i], &buff[0], 1);
        BME280_reg_read((dir_CPress[i])+1, &buff[1], 1);
        APress.values[i] = (uint16_t) ((buff[1] << 8) | buff[0]);
    }*/

    BME280_reg_read(dig_H1, &buff[0], 1);
    AHum.valuesu8[0] = buff[0]; // calib_dig_h1

    BME280_reg_read(dig_H2_LSB, &buff[0], 1);
    AHum.valuesu16[0] = buff[0]; // calib_dig_h2_lsb

    BME280_reg_read(dig_H2_MSB, &buff[1], 1);
    AHum.valuesu16[0] |= (buff[1] << 8); // calib_dig_h2_msb

    BME280_reg_read(dig_H3, &buff[0], 1);
    AHum.valuesu8[1] = buff[0]; // calib_dig_h3

    BME280_reg_read(dig_H4_LSB, &buff[0], 1);
    AHum.valuesu16[1] = (buff[0] & 0xF); // calib_dig_h4_lsb

    BME280_reg_read(dig_H4_MSB, &buff[1], 1);
    AHum.valuesu16[1] |= (buff[1] << 4); // calib_dig_h4_lsb

    BME280_reg_read(dig_H5_LSB, &buff[0], 1);
    AHum.valuesu16[2] = (buff[0] >> 4); // calib_dig_h5_lsb
    BME280_reg_read(dig_H5_MSB, &buff[1], 1);
    AHum.valuesu16[2] |= (buff[1] << 4); // calib_dig_h5_msb

    BME280_reg_read(dig_H6, &buff[0], 1);
    AHum.valuesu8[2] = buff[0]; // calib_dig_h1
    return;
}

void BME280_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    BME280_reg_read(reg_addr, data, len);
}

void BME280_write_byte(uint8_t reg_addr, uint8_t data)
{
    BME280_reg_write_byte(reg_addr, data);
}


uint32_t read_Temp()
{
    uint8_t buff;
    uint32_t temp;

    // 98765432 10987654 3210
    BME280_reg_read(TEMP_MSB, &buff, 1);
    temp = (buff << 12);
    BME280_reg_read(TEMP_LSB, &buff, 1);
    temp |= (buff << 4);
    BME280_reg_read(TEMP_XLS, &buff, 1);
    temp |= (buff >> 4);
    return temp;
}

uint32_t read_Hum()
{
    uint8_t buff;
    uint32_t hum;

    // 98765432 10987654 3210
    BME280_reg_read(HUM_MSB, &buff, 1);
    hum = (buff << 8);
    BME280_reg_read(HUM_LSB, &buff, 1);
    hum |= (buff);
    return hum;
}

/*
uint32_t read_Press()
{
    uint8_t buff;
    uint32_t press;

    // 98765432 10987654 3210
    BME280_reg_read(PRESS_MSB, &buff, 1);
    press = (buff << 12);
    BME280_reg_read(PRESS_LSB, &buff, 1);
    press |= (buff << 4);
    BME280_reg_read(PRESS_XLS, &buff, 1);
    press |= (buff >> 4);
    return press;
}

void calib_Press(BME280_t *sens)
{
    int32_t uncalibPress = read_Press();
    int64_t var1, var2, p;
    
    var1 = ((int64_t)sens->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)APress.values[5];
    var2 = var2 + ((var1 * (int64_t)APress.values[4]) << 17);
    var2 = var2 + (((int64_t)APress.values[3]) << 35);
    var1 = ((var1 * var1 * (int64_t)APress.values[2]) >> 8) + ((var1 * (int64_t)APress.values[1]) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)APress.values[0]) >> 33;

    if(var1 == 0)
    {

        return;
    }
    p = 1048576 - uncalibPress;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)APress.values[8]) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)APress.values[7]) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)APress.values[6]) << 4);
    sens->press = (float)(p / 256);
}*/

void calib_Temp(BME280_t *sens)
{
    int32_t uncalibTemp = (int32_t) read_Temp();
    int32_t var1, var2;
    
    var1 = ((((uncalibTemp >> 3) - ((int32_t)ATemp.values[0] << 1))) * ((int32_t)ATemp.values[1])) >> 11;
    var2 = (((((uncalibTemp >> 4) - ((int32_t)ATemp.values[0])) * ((uncalibTemp >> 4) - ((int32_t)ATemp.values[0]))) >> 12) * ((int32_t)ATemp.values[2])) >> 14;
    sens->t_fine = var1 + var2;
    sens->temp = (sens->t_fine * 5 + 128) >> 8;
    sens->temp /= 100;

}

void calib_Hum(BME280_t *sens)
{
    int32_t var1, uncalibHum = read_Hum();

    var1 = (sens->t_fine - ((int32_t)76800));
    var1 = (((((uncalibHum << 14) - (((int32_t)AHum.valuesu16[1]) << 20) - (((int32_t)AHum.valuesu16[2]) * 
            var1)) + ((int32_t)16384)) >> 15) * (((((((var1 *
            ((int32_t)AHum.valuesu8[2])) >> 10) * (((var1 * ((int32_t) AHum.valuesu8[1])) >> 11) +
            ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)AHum.valuesu16[0]) +
            8192) >> 14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)AHum.valuesu8[0])) >> 4));
    var1 = (var1 < 0 ? 0 : var1);
    var1 = (var1 > 419430400 ? 419430400 : var1);
    var1 = (uint32_t)(var1 >> 12);

    sens->humedad = (float)(var1 / 1024);
    
}

void init_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);
}

void set_led(bool state)
{
    if(state == true)
    {
        gpio_set_level(LED, 1);
    }
    else
    {
        gpio_set_level(LED, 0);
    }
}

static void read_BME280(void *arg)
{
    BME280_t *BME280 = (BME280_t *)arg;
    BME280_write_byte(BME280_DIR_CRT_M, (uint8_t) (1 << 7 | 1 << 5 | 1 << 4 | 1 << 2 | 1));
    BME280_read(BME280_DIR_ID, (uint8_t *) (&BME280->whoami), 1);
    
    while(1)
    {
        calib_Temp(BME280);
        //calib_Press(BME280);
        calib_Hum(BME280);

        BME280_write_byte(BME280_DIR_CRT_M, (uint8_t) (1 << 7 | 1 << 5 | 1 << 4 | 1 << 2 | 1));
        if(BME280->state == false)
        {
            vTaskSuspend(BME280->task);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
    }
    vTaskDelete(NULL);
}

#endif