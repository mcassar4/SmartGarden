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

    const int update_loop = 100; // milliseconds
    const int second_delay = 1000/update_loop; // represents 1 second in delay
    const int minute_delay = second_delay * 60; // represents 1 minute in delay
    const int max_loop_lifetime = minute_delay * 60 * 24; // (24 hours)

    const int update_display_interval = second_delay * 1; // (1 second)
    const int tx_to_webserver_interval = second_delay * 5; // (5 sec)
    const int reconnect_interval = second_delay * 10; // (10 seconds)
    const int log_stats_interval = minute_delay * 5; // (5 minutes)
    
    unsigned loop_lifetime = 0;
    while (true) {
        loop_lifetime++;
    
        bool do_log_stats = loop_lifetime % log_stats_interval == 0;
        bool do_tx_to_webserver = loop_lifetime % tx_to_webserver_interval == 0;
        bool do_reset_loop_lifetime = loop_lifetime >= max_loop_lifetime;
        bool do_update_display = loop_lifetime % update_display_interval == 0;
        bool do_reconnect_attempt = loop_lifetime % reconnect_interval == 0;

        // Operations regardless of server connection
        heltec_loop(); // Check for button press
        if (do_update_display) { display_stats(); }
        if (do_log_stats) { log_stats(); }

        // Operations dependant on server connection
        if (wifi_manager_is_connected()) {
            if (do_tx_to_webserver) {}
        }
        else if (do_reconnect_attempt)
        {
            ESP_LOGI(LOOP_LOG_TAG, "WiFi disconnected, attempting to reconnect...");
            wifi_manager_reconnect();
        }

        // Reset loop_lifetime every day to avoid overflow in long term
        if (do_reset_loop_lifetime) {
            loop_lifetime = 0;
            ESP_LOGI(GARDEN_LOG_TAG, "Reset Loop");
        }
        vTaskDelay(pdMS_TO_TICKS(update_loop)); // Delay in loop
    }
}

