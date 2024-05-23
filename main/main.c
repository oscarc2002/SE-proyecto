#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BME280C.h"
#include "Wifi.h"
#include "Config.h"
#include <esp_http_server.h>



void app_main(void)
{
    
#if (MODOESP == MASTER)
    httpd_handle_t server = NULL;
    init_my_wifi(&server);

#elif (MODOESP == SLAVE1)
    Utils_t utils;
    init_slave1(&utils);

#endif

}