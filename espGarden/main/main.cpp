#include "heltec_unofficial.h"

// // Define the structure for a watering zone
// struct WateringZone {
//     std::string name;
//     bool is_open;
//     int timer; // in seconds
// };

// // Define the structure for the system state
// struct SystemState {
//     std::vector<WateringZone> zones;
//     std::queue<std::string> command_queue;
//     std::vector<SystemState> snapshot_buffer;
//     SemaphoreHandle_t state_mutex;
// };

// static SystemState system_state;
ssd1306_handle_t display;


extern "C" void app_main() {
    heltec_setup(); // Main Setup

    esp_log_level_set(GARDEN_LOG_TAG, ESP_LOG_INFO);
    ESP_LOGI(GARDEN_LOG_TAG, "Starting Heltec Garden App");

    // init_system_state(system_state);

    const int loop_delay = 60; // seconds
    while (true) {
        heltec_loop(loop_delay * 1000);
        // check_zone_status();
        // update_webpage()
    }
}
