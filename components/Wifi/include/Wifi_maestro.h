#ifndef WIFI_MAESTRO_H__
#define WIFI_MAESTRO_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include "Wifi.h"

void init_my_wifi(httpd_handle_t *server);

void cont_index_html(void);

#endif