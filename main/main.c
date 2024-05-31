#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "BME280C.h"
#include "Wifi_maestro.h"
#include "Config.h"
#include "Sockets_cliente.h"



void app_main(void)
{
    
#if (MODOESP == MAESTRO)
    static httpd_handle_t server = NULL;
    init_master(&server);

#elif (MODOESP == SLAVE1)
    Utils_t utils;
    slave_state_t slave1;
    slave_hd = &slave1;
    
    init_slave1(&utils, &slave1);
    while(1)
    {   
        if(slave1.connect_wf == true)
        {
            send_msj(&utils);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

#endif
}

