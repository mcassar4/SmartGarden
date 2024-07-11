#include <stdio.h>
#include <inttypes.h>
#include "esp_chip_info.h"
#include "heltec_unofficial.h"
#include "nvs_flash.h"
#include "wifi_manager.h"

// Define logging tag
static const char* LOG_TAG = "HeltecGardenApp";

// LoRa configuration parameters
#define FREQUENCY 915
#define BANDWIDTH 250.0
#define SPREADING_FACTOR 9
#define TRANSMIT_POWER 0

ssd1306_handle_t ssd1306_dev;

extern "C" void app_main() {
    // Initialize serial logging
    esp_log_level_set(LOG_TAG, ESP_LOG_INFO);
    ESP_LOGI(LOG_TAG, "Starting app_main");

    // Setup Heltec device (OLED and LoRa)
    heltec_setup();
    // needed for wifi
    ESP_ERROR_CHECK(nvs_flash_init());

    const char *ssid = "Test";
    const char *pass = "123456789";
    wifi_init(ssid, pass);
    int32_t rssi;
    char rssi_str[20];
    
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t*)"Initialization OK", 12, 1);
    ssd1306_refresh_gram(ssd1306_dev);

    while (true) {
        // Simple delay to keep the task alive
        vTaskDelay(pdMS_TO_TICKS(1000));

        // Get the RSSI value
        rssi = wifi_manager_get_rssi();

        // Convert RSSI value to string
        sprintf(rssi_str, "RSSI: %ld", rssi);

        // Clear the screen and display the RSSI value
        ssd1306_clear_screen(ssd1306_dev, 0x00);
        ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t*)rssi_str, 12, 1);
        ssd1306_refresh_gram(ssd1306_dev);
        //heltec_loop();
    }
}
