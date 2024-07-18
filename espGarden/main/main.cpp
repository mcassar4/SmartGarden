#include <algorithm>
#include <stdio.h>
#include <inttypes.h>

#include "heltec_unofficial.h"

ssd1306_handle_t display;


extern "C" void app_main() {
    heltec_setup();

    esp_log_level_set(GARDEN_LOG_TAG, ESP_LOG_INFO);
    ESP_LOGI(GARDEN_LOG_TAG, "Starting Heltec Garden App");

    const int loop_delay = 100; //ms
    while (true) {
        heltec_loop(loop_delay);
    }
}
