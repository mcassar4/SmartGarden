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
        {"Z4", GPIO4, false, 0}  // Zone 4
    };
    std::queue<std::string> command_queue;
    SemaphoreHandle_t state_mutex;
};

static SystemState system_state;


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