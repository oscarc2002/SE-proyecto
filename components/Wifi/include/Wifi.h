#ifndef _WIFI_H__
#define _WIFI_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_http_server.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include <esp_system.h>
#include <sys/param.h>
#include <string.h>


#include "BME280C.h"
#include "driver/gpio.h"
#include <stdio.h>
#include "sdkconfig.h"

#define EXAMPLE_ESP_WIFI_SSID      "MyESP32"
#define EXAMPLE_ESP_WIFI_PASS      "SE_12345678"
#define EXAMPLE_MAX_STA_CONN       10

typedef struct Utils_tag
{
    sensor_value_t BME280;
    bool ledState;
    const char *ledState_char;
}Utils_t;

void cont_index_html(void);

void init_my_wifi(httpd_handle_t *server);

esp_err_t wifi_init_softap(void);

#endif
