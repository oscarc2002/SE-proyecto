#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

#define LED_RED 13
#define LED_BLUE 12
#define LED_YELLOW 27
#define LED_GREEN 14

float temperature = 0.0f;
uint16_t humidity = 0;

static void configure_led(void)
{
    gpio_reset_pin(LED_RED);
    gpio_reset_pin(LED_BLUE);
    gpio_reset_pin(LED_YELLOW);
    gpio_reset_pin(LED_GREEN);

    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_YELLOW, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
}

void change_led(){
    //Temperatura
    if(temperature > 25){
        gpio_set_level(LED_RED, 1);
        gpio_set_level(LED_BLUE, 0);
    }
    else if(temperature < 20){
        gpio_set_level(LED_RED, 0);
        gpio_set_level(LED_BLUE, 1);
    }
    else{
        gpio_set_level(LED_RED, 0);
        gpio_set_level(LED_BLUE, 0);
    }

    //Humedad
    if(humidity > 50){
        gpio_set_level(LED_GREEN, 1);
        gpio_set_level(LED_YELLOW, 0);
    }
    else if(humidity < 10){
        gpio_set_level(LED_GREEN, 0);
        gpio_set_level(LED_YELLOW, 1);
    }
    else{
        gpio_set_level(LED_GREEN, 0);
        gpio_set_level(LED_YELLOW, 0);
    }
}

void app_main(void)
{
    configure_led();

    while (1) {
        
        /* Toggle the LED state */
        change_led();
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
