#include <stdio.h>
#include "Sockets_cliente.h"

const char *TAG = "Slave1";
const char *host = "192.168.4.1";
const char *uri = "/gatewey_s1";

#if(MODOESP == SLAVE1)
static void http_native_request(paquete_t *pkt)
{
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};   // Buffer to store response of http request
    int content_length = 0;
    esp_http_client_config_t config = {
        .host = host,
        .url = "/gatewey_s1",
        .method = HTTP_METHOD_POST,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    char post[sizeof(paquete_t) + 1];
    esp_err_t err;
    // POST Request
    memcpy(post, pkt, sizeof(paquete_t));
    post[sizeof(paquete_t)] = 0;
    esp_http_client_set_header(client, "Content-Type", "application/octet-stream");
    err = esp_http_client_open(client, sizeof(post));

    if (err != ESP_OK) 
    {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    }
    else 
    {
        int wlen = esp_http_client_write(client, post, sizeof(post));
        
        if (wlen < 0) {
            ESP_LOGE(TAG, "Write failed");
        }
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %"PRIu64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, sizeof(post));
            } else {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }
    esp_http_client_cleanup(client);
}

void send_msj(Utils_t *util)
{
    paquete_t pkt;
    pkt.header = 0xB280;
    pkt.len = 8;
    pkt.dato1 = (uint16_t)util->BME280.temp;
    pkt.dato2 = (uint16_t)util->BME280.humedad;
    //crc32_le(&pkt);
    http_native_request(&pkt);
}
/*
static void get_gatewey(){

    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};   // Buffer to store response of http request
    int content_length = 0;
    esp_http_client_config_t config = {
        .host = host,
        .url = "/gatewey_s1",
        .method = HTTP_METHOD_PATCH
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);
    strcpy(output_buffer, "Sisi que chingue a su madre");

    esp_http_client_set_method(client, HTTP_METHOD_GET);
    esp_err_t err = esp_http_client_open(client, 0);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0) {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        } else {
            int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0) {
                ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %"PRIu64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, data_read);
            } else {
                ESP_LOGE(TAG, "Failed to read response");
            }
        }
    }
    esp_http_client_close(client);
}
*/

#endif