idf_component_register(SRCS "BME280C.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "driver" "freertos" "Config" "Wifi")
