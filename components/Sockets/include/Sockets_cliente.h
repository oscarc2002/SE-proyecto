#ifndef _SOCKETSC_H__
#define _SOCKETSC_H__
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include "esp_err.h"
#include "esp_http_client.h"
#include "BME280C_types.h"
#include "paquete.h"

#define MAX_HTTP_OUTPUT_BUFFER 1024

void send_msj(Utils_t *util);

#endif