#include <stdio.h>
#include "Wifi.h"
#include "config.h"



static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#if (MODOESP == MAESTRO)
static const char *TAG = "softAP_WebServer";

static char *view_html = NULL;
static Utils_t utils = {0};
static httpd_handle_t start_webserver(void);
static esp_err_t hello_get_handler(httpd_req_t *req);

static const httpd_uri_t hello = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = hello_get_handler,
};

void cont_index_html(void)
{
    extern unsigned char view_start[] asm("_binary_index_html_start");
    extern unsigned char view_end[] asm("_binary_index_html_end");
    size_t view_len = view_end - view_start;
    view_html = malloc(view_len);
    memcpy(view_html, view_start, view_len);

}

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Iniciar el servidor httpd 
    ESP_LOGI(TAG, "Iniciando el servidor en el puerto: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Manejadores de URI
        ESP_LOGI(TAG, "Registrando manejadores de URI");
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    //ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}


/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    static float temp = 0, hum = 0, press = 0;
    char *buf, *format_html;
    size_t buf_len;
    ESP_LOGI(TAG, "URI: %s", req->uri);
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }
    
    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            if(!strcmp(req->uri, "/?led1Toggle"))
            {
                utils.ledState = !utils.ledState;
            }
            else if(!strcmp(req->uri, "/?led1Pas"))
            {
                if(utils.ledState)
                    utils.ledState_char = On;
                else
                    utils.ledState_char = Off;
            }
            else if(!strcmp(req->uri, "/?BM280Off"))
            {
                vTaskSuspend(utils.BME280.task);
            }
            else if(!strcmp(req->uri, "/?BM280On"))
            {
                utils.BME280.state = true;
                vTaskResume(utils.BME280.task);
            }
            else if(!strcmp(req->uri, "/?BM280Refres"))
            {   
                hum = utils.BME280.humedad;
                temp = utils.BME280.temp;
                press = utils.BME280.press;
            }
            else
            {
                ESP_LOGI(TAG, "Tarea no encontrada");
                return ESP_FAIL;
            }
        }
        free(buf);
    }
    asprintf(&format_html, view_html, utils.ledState_char,(int)utils.BME280.whoami, temp, hum, press);

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    //const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)format_html, HTTPD_RESP_USE_STRLEN);
    free(format_html);
    set_led(utils.ledState);
    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    return ESP_OK;
}
#else
    static const char *TAG = "wifi_sta_slave1";
#endif

void init_my_wifi(httpd_handle_t *server)
{

    ESP_LOGI(TAG, "init softAP");
    ESP_ERROR_CHECK(init_wifi());
#if (MODOESP == MAESTRO)    
    cont_index_html();

    *server = start_webserver();
#else
    *server = NULL;
#endif
}

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
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

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
    
    ESP_LOGI(TAG, "Inicializacion de softAP terminada. SSID: %s password: %s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
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
    if(event_id == WIFI_EVENT_STA_CONNECTED)
    {
        wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_base;
        ESP_LOGI(TAG, "AP %s conectado, channel=%d AID=%d", (char *)event->ssid, event->channel, event->aid); 
    }
    else if(event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        wifi_event_sta_disconnected_t *event = (wifi_event_sta_disconnected_t *)event_base;
        ESP_LOGI(TAG, "AP %s desconectado, Reason=%d RSSI=%d", (char *)event->ssid, event->reason, event->rssi);
    }

#endif
}