idf_component_register(SRCS "DHT22.c"
                       REQUIRES driver
                       REQUIRES nvs_flash
                       INCLUDE_DIRS ".")
