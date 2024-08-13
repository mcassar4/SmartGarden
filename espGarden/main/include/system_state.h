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



#endif // SYSTEM_STATE_H