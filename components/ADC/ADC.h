#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <string.h>

#define REFERENCE_MV (3300)
#define ADC_SIZE (4096)

void configADC();

int readADC();

#endif