#include <stdio.h>
#include "ADC.h"

void configADC(){
    adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_BITWIDTH_12);
}

int readADC(){
    return adc1_get_raw(ADC_CHANNEL_6);
}