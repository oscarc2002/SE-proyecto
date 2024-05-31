#include <stdio.h>
#include "Wifi.h"
#include "config.h"

esp_ip4_addr_t const *ip_addr = NULL;

#if(MODOESP == MAESTRO)
    static const char *TAG = "softAP_WebServer";
#else
    static const char *TAG = "wifi_sta_slave1";
    slave_state_t *slave_hd = NULL;
#endif
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

esp_err_t init_wifi(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    

    #if(MODOESP == MAESTRO)
    esp_netif_create_default_wifi_ap();
    
    #else
    esp_netif_create_default_wifi_sta();
    #endif

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

    #if(MODOESP == MAESTRO)
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "Inicializacion de softAP terminada. SSID: %s password: %s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    #else
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .channel = 0
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    #endif
    
    
    return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    #if(MODOESP == MAESTRO)
    if (event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "estacion %X:%X:%X:%X:%X:%X se unio, AID=%d", event->mac[5], event->mac[4],event->mac[3],event->mac[2],event->mac[1],event->mac[0], event->aid);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) 
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "estacion %X:%X:%X:%X:%X:%X se desconecto, AID=%d", event->mac[5], event->mac[4],event->mac[3],event->mac[2],event->mac[1],event->mac[0], event->aid);
    }
    #else

    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_base;
        ESP_LOGI(TAG, "AP %s conectado, channel=%d AID=%d", (char *)event->ssid, event->channel, event->aid);
        ip_addr = NULL;
        
    }

    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {   
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_base;
        ESP_LOGI(TAG, "AP %s desconectado, Reason=%d RSSI=%d", (char *)event->ssid, event->reason, event->rssi);
        ESP_ERROR_CHECK(esp_wifi_connect());
        slave_hd->connect_wf = false;
    }

    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        static esp_ip4_addr_t sta_addr = {0};
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_base;

        sta_addr = event->ip_info.ip;
        ip_addr = &sta_addr;
        ESP_LOGI("IP address 0", IPSTR, IP2STR(&sta_addr));
        ESP_LOGI("IP address 1", "%" PRIx32 "\n", event->ip_info.ip.addr);
        slave_hd->connect_wf = true;
    }

#endif
}