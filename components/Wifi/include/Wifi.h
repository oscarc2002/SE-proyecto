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

extern esp_ip4_addr_t const *ip_addr;

esp_err_t init_wifi(void);

#endif
