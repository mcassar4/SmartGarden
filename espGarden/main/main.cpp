#include "heltec_unofficial.h"

#include "system_state.h"

void openZone(WateringZone& zone) {
    gpio_set_level((gpio_num_t)zone.pin, HIGH);
    zone.is_open = true;
    ESP_LOGI(GARDEN_LOG_TAG, "Opened zone %s for %d minutes", zone.name.c_str(), zone.timer);
}

void closeZone(WateringZone& zone) {
    gpio_set_level((gpio_num_t)zone.pin, LOW);
    zone.is_open = false;
    ESP_LOGI(GARDEN_LOG_TAG, "Closed zone %s", zone.name.c_str());
}

bool process_watering_command(const std::string& command) {
    // Command is verified by having a colon (dangerous)
    size_t delimiter_pos = command.find(':');
    if (delimiter_pos != std::string::npos) {
        int zone_number = std::stoi(command.substr(0, delimiter_pos));
        int time = std::stoi(command.substr(delimiter_pos + 1));

        // Verify correct zone number
        if (zone_number >= 1 && zone_number <= system_state.zones.size()) {
            WateringZone& zone = system_state.zones[zone_number - 1];  // Zone index
            zone.timer = time;
            openZone(zone);
            return true;
        } else {
            ESP_LOGE(GARDEN_LOG_TAG, "Attempted to process invalid zone number: %d", zone_number);
        }
    } else {
        ESP_LOGE(GARDEN_LOG_TAG, "Attempted to process invalid command format: %s", command.c_str());
    }
    return false;
}

extern "C" void app_main() {
    heltec_setup(); // Main Setup

    esp_log_level_set(GARDEN_LOG_TAG, ESP_LOG_INFO);
    ESP_LOGI(GARDEN_LOG_TAG, "Starting Heltec Garden App");

    const int update_loop       = 100;                          // milliseconds
    const int second_delay      = 1000/update_loop;             // represents 1 second in delay
    const int minute_delay      = second_delay * 60;            // represents 1 minute in delay
    const int max_loop_lifetime = minute_delay * 60 * 24;       // represents 24 hours in delay

    const int update_display_interval   = second_delay * 1;     // (1 second)
    const int reconnect_interval        = second_delay * 10;    // (10 seconds)
    const int check_state_interval      = minute_delay * 1;     // (1 minute)   DONT CHANGE
    const int log_stats_interval        = minute_delay * 5;     // (5 minutes)
    
    unsigned loop_lifetime = 0;
    while (true) {
        loop_lifetime++;
    
        bool do_reset_loop_lifetime = loop_lifetime >=  max_loop_lifetime;
        bool do_log_stats           = loop_lifetime %   log_stats_interval      == 0;
        bool do_check_state         = loop_lifetime %   check_state_interval    == 0;
        bool do_update_display      = loop_lifetime %   update_display_interval == 0;
        bool do_reconnect_attempt   = loop_lifetime %   reconnect_interval      == 0;

        // Operations regardless of server connection
        heltec_loop(); // Check for button press
        if (do_update_display) { display_stats(); }
        if (do_log_stats) { log_stats(); }
        if (do_check_state) {
            // Lock the system state for checking
            xSemaphoreTake(system_state.state_mutex, portMAX_DELAY);

            // Watering commands will only be processed if
            // the system isnt currently watering and there is a command in the queue
            if (!system_state.is_watering &&
                !system_state.command_queue.empty())
            {
                // Process the first command in the queue
                std::string command = system_state.command_queue.front();
                system_state.command_queue.pop();
                // Handle command parsing and GPIO zone control logic
                if (process_watering_command(command)) {
                    system_state.is_watering = true;
                }

            // Update the timer for each zone every check_state_interval, close valve if it reaches 0
            } else if (system_state.is_watering) {
                for (auto& zone : system_state.zones) {
                    if (zone.is_open) {
                        if (zone.timer > 0) {
                            // If you update the check_state_interval, you must update this too
                            // since zone.timer is represented in minutes
                            zone.timer--;
                        } else {
                            closeZone(zone);
                            system_state.is_watering = false;
                        }
                    }
                }
            }

            // Unlock the system state
            xSemaphoreGive(system_state.state_mutex);
        }
        
        if (do_reconnect_attempt && !wifi_manager_is_connected()) {
           ESP_LOGI(LOOP_LOG_TAG, "WiFi disconnected, attempting to reconnect...");
           esp_wifi_connect();
        }

        // Reset loop_lifetime every day to avoid overflow in long term
        if (do_reset_loop_lifetime) {
            loop_lifetime = 0;
            ESP_LOGI(GARDEN_LOG_TAG, "Reset Loop");
        }
        vTaskDelay(pdMS_TO_TICKS(update_loop)); // Delay in loop
    }
}

