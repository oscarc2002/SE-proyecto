idf_component_register(SRCS "Wifi.c"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES "index.html"
                    REQUIRES "nvs_flash" "esp_wifi" "esp_event" "esp_eth" "esp_netif" "esp_http_server" "esp-tls" "esp_system" "BME280C")
