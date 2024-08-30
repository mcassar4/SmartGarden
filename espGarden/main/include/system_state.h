#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

// Define the structure for a watering zone
struct WateringZone {
    std::string name;
    unsigned pin;
    bool is_open;
    unsigned timer; // in minutes
};

// Define the structure for the system state
struct SystemState {
    bool is_watering = false;
    std::vector<WateringZone> zones = {
        {"Z1", GPIO7, false, 0}, // Zone 1
        {"Z2", GPIO6, false, 0}, // Zone 2
        {"Z3", GPIO5, false, 0}, // Zone 3
        {"Z4", GPIO4, false, 0}, // Zone 4
        {"Z5", GPIO3, false, 0}  // Zone 5
    };
    std::queue<std::string> command_queue;
    SemaphoreHandle_t state_mutex;
};

//queue helper function
void qclear(std::queue<std::string> &queue){
    std::queue<std::string> empty;
    std::swap(queue, empty);
}

static SystemState system_state;


bool add_watering_command_to_queue(std::string wateringCommand) {
    // Command is verified by having a colon
    size_t delimiter_pos = wateringCommand.find(':');
    if (delimiter_pos != std::string::npos) {
        std::string zone_str = wateringCommand.substr(0, delimiter_pos);
        std::string time_str = wateringCommand.substr(delimiter_pos + 1);

        try {
            // Maximum number of hours for a zone to be open is 4
            // Minimum number of minutes for a zone to be open is 1 minute
            if (std::stoi(time_str) > 4*60 || std::stoi(time_str) < 1) {
                ESP_LOGE(HTTP_LOG_TAG, "Watering time invalid! %s.", wateringCommand.c_str());
                return false;
            }
        } catch(...) {
            ESP_LOGE(HTTP_LOG_TAG, "Invalid watering time! %s.", wateringCommand.c_str());
            return false;
        }


        std::string curr_zone_command;
        if (zone_str == "A") {
            unsigned curr_zone_num = 1;
            for (WateringZone& zone: system_state.zones)
            {
                curr_zone_command = std::to_string(curr_zone_num) + ":" + time_str;
                system_state.command_queue.push(curr_zone_command);
                ESP_LOGI(HTTP_LOG_TAG, "Added command to queue: %s", curr_zone_command.c_str());
                curr_zone_num++;
            }
            return true;
        } else {
            try {
                if (std::stoi(zone_str) >= 1 && std::stoi(zone_str) <= system_state.zones.size()) {
                    curr_zone_command = zone_str + ":" + time_str;
                    system_state.command_queue.push(curr_zone_command);
                    ESP_LOGI(HTTP_LOG_TAG, "Added command to queue: %s", curr_zone_command.c_str());
                    return true;
                } else {
                    ESP_LOGE(HTTP_LOG_TAG, "Invalid zone number received %s.", wateringCommand.c_str());
                    return false;
                }
            } catch (...) {
                ESP_LOGE(HTTP_LOG_TAG, "Invalid command received %s.", wateringCommand.c_str());
                return false;

            }
        }
    //TODO: change stop command to form [stop:{number}], where number encodes which zones to close.
    //      this would allow all the stop command processing to be moved main/main.cpp
    //      ([stop:] as the command format would also allow this but might as well add some functionality)
    } else if (wateringCommand.compare("stop") >= 0) {    //close all zones on stop command
        qclear(system_state.command_queue);
        system_state.command_queue.push("stop");
        ESP_LOGE(HTTP_LOG_TAG, "Stopped watering and cleared queue from command %s.", wateringCommand.c_str());  
    }   

    ESP_LOGE(HTTP_LOG_TAG, "Invalid Command Received %s.", wateringCommand.c_str());
    return false;
}

//################################################################################//
// Json Construction

std::string boolToString(bool value) {
    return value ? "true" : "false";
}

std::string escapeJsonString(const std::string &input) {
    std::ostringstream ss;
    for (char c : input) {
        switch (c) {
            case '\"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

// Function to convert the state to JSON
std::string stateToJson() {
    xSemaphoreTake(system_state.state_mutex, portMAX_DELAY);

    std::ostringstream json;
    json << "{";

    json << "\"is_watering\":" << boolToString(system_state.is_watering) << ",";

    json << "\"queue\":[";
    std::queue<std::string> temp_queue = system_state.command_queue;
    bool first = true;
    while (!temp_queue.empty()) {
        if (!first) {
            json << ",";
        }
        json << "\"" << escapeJsonString(temp_queue.front()) << "\"";
        temp_queue.pop();
        first = false;
    }
    json << "],";

    for (size_t i = 0; i < system_state.zones.size(); ++i) {
        const WateringZone &zone = system_state.zones[i];
        std::string zone_name = "Z" + std::to_string(i + 1);
        json << "\"" << zone_name << "\":{";
        json << "\"is_open\":" << boolToString(zone.is_open) << ",";
        json << "\"timer\":" << zone.timer;
        json << "}";
        if (i != system_state.zones.size() - 1) {
            json << ",";
        }
    }

    json << "}";

    xSemaphoreGive(system_state.state_mutex);
    return json.str();
}

#endif // SYSTEM_STATE_H
