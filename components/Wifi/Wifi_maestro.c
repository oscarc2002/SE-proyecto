#include "Wifi_maestro.h"

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

void init_my_wifi(httpd_handle_t *server)
{

    ESP_LOGI(TAG, "init softAP");
    ESP_ERROR_CHECK(init_wifi());   
    cont_index_html();

    *server = start_webserver();
}

#else
    static const char *TAG = "wifi_sta_slave1";
#endif
