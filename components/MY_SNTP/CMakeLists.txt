idf_component_register(SRCS "my_sntp.c"
                       REQUIRES esp_event
                       REQUIRES nvs_flash
                       REQUIRES protocol_examples_common
                       REQUIRES esp_netif
                       INCLUDE_DIRS ".")
