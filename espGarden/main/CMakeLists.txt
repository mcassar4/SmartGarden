idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "./include"
                    REQUIRES ssd1306 RadioLib esp_wifi nvs_flash)
