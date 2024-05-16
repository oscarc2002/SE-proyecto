#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BME280C.h"
#include "Wifi.h"
#include <esp_http_server.h>



void app_main(void)
{
    
    httpd_handle_t server = NULL;
    init_my_wifi(&server);
    
    

}