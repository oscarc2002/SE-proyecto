#ifndef WIFI_MAESTRO_H__
#define WIFI_MAESTRO_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include "Wifi.h"
#include "paquete.h"

void init_master(httpd_handle_t *server);

void cont_index_html(void);

#endif