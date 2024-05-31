#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include <esp_https_server.h>
#include "esp_tls.h"
#include <string.h>
#include "driver/gpio.h"
#include <stdio.h>
#include "driver/i2c.h"
#include "i2c_sensor.c"

#define led 5

int8_t led_state = 0;
char led_state_b [50];
uint8_t firstTime = 1;

static const char *TAG = "Web Service";

esp_err_t init_led(void);
esp_err_t toggle_led(void);

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern unsigned char view_start[] asm("_binary_view_html_start");
    extern unsigned char view_end[] asm("_binary_view_html_end");
    size_t view_len = view_end - view_start;
    char viewHtml[view_len];
    memcpy(viewHtml, view_start, view_len);
    ESP_LOGI(TAG, "URI: %s", req->uri);
    char *viewHtmlUpdated;
    int formattedStrResult = 0;

    static uint8_t who_b = 0;
    static float pressure_b = 0;
    static float temperature_b = 0;
    static uint32_t humidity_b = 0;

    if(firstTime)
    {
        firstTime = 0;
        if(led_state)
            strcpy(led_state_b, "ENCENDIDO");
        else
            strcpy(led_state_b, "APAGADO");
        who_b = whoAmI();
        humidity_b = readHumidity();
        temperature_b = readTemperature();
        pressure_b = readPressure();
    }
    else if (strcmp(req->uri, "/?led") == 0)
    {
        toggle_led();
    }
    else if (strcmp(req->uri, "/?edoled") == 0)
    {
        if(led_state)
            strcpy(led_state_b, "ENCENDIDO");
        else
            strcpy(led_state_b, "APAGADO");
    }
    else if(strcmp(req->uri, "/?activar") == 0)
    {
        ESP_ERROR_CHECK(BMP280_register_write_byte(BMP280_CTRL_MEAS, ((3 << 5) | (3 << 2) | (3 << 0)))); //Normal mode
    }
    else if(strcmp(req->uri, "/?desactivar") == 0)
    {
        ESP_ERROR_CHECK(BMP280_register_write_byte(BMP280_CTRL_MEAS, ((3 << 5) | (3 << 2)))); //Sleep mode
    }
    else if(strcmp(req->uri, "/?actualizar") == 0)
    {
        who_b = whoAmI();
        humidity_b = readHumidity();
        temperature_b = readTemperature();
        pressure_b = readPressure();
    }

    formattedStrResult = asprintf(&viewHtmlUpdated, viewHtml, led_state_b, who_b, humidity_b, temperature_b, pressure_b);

    httpd_resp_set_type(req, "text/html");

    if (formattedStrResult > 0)
    {
        httpd_resp_send(req, viewHtmlUpdated, view_len);
        free(viewHtmlUpdated);
    }
    else
    {
        ESP_LOGE(TAG, "Error actualizando variables");
        httpd_resp_send(req, viewHtml, view_len);
    }

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Iniciando el servidor");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error iniciando el servidor!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registrando los handlers del URI");
    httpd_register_uri_handler(server, &root);
    return server;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_ssl_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = start_webserver();
    }
}

esp_err_t init_led(void)
{
    gpio_reset_pin(led); //Reset out ports
    gpio_set_direction(led, GPIO_MODE_DEF_OUTPUT);
    ESP_LOGI(TAG, "init led completed");
    return ESP_OK;
}

esp_err_t toggle_led()
{
    int8_t state = 0;
    led_state = !led_state;
    state = led_state;
    gpio_set_level(led, state);
    return ESP_OK;
}

void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    //Configurar BMP280
    ESP_ERROR_CHECK(BMP280_register_write_byte(BMP280_CTRL_HUM, 3)); //Overslamping humidity x 4
    ESP_ERROR_CHECK(BMP280_register_write_byte(BMP280_CTRL_MEAS, ((3 << 5) | (3 << 2) | (3 << 0)))); //Overslamping temperature and pression x 4, normal mode

    ESP_ERROR_CHECK(init_led());
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    ESP_ERROR_CHECK(example_connect());
}
