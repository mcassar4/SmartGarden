idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "./include"
                    REQUIRES ssd1306 RadioLib esp_wifi esp_event nvs_flash esp_timer esp_adc)
