#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

EventGroupHandle_t wifi_event_group;
esp_netif_t *sta_netif = NULL;
const int CONNECTED_BIT = BIT0;

// Event handler for WIFI and IP events
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

bool wifi_manager_is_connected() {
    return (xEventGroupGetBits(wifi_event_group) & CONNECTED_BIT) != 0;
}

int32_t wifi_manager_get_rssi(void) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return ap_info.rssi;
    } else {
        ESP_LOGE(WIFI_LOG_TAG, "Failed to get AP info");
        return 0;  // Return 0 if unable to get RSSI
    }
}

esp_err_t wifi_manager_get_ip(char* ip){
    esp_netif_ip_info_t net_info;
    memset(&net_info, 0, sizeof(esp_netif_ip_info_t));
    if (esp_netif_get_ip_info(sta_netif, &net_info) == 0) 
        sprintf(ip, "" IPSTR, IP2STR(&net_info.ip));  
    return ESP_OK;
}

// Task prints 
void wifi_print_stats(void *pvParameters) {
    wifi_ap_record_t ap_info;
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));

    // Refresh the wifi setup until its connected
    while (1) {
        // Check for connection every 5 seconds
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        bool is_connected = (esp_netif_get_ip_info(sta_netif, &ip) == 0 && esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK);

        // Check/print connection every 10 minutes
        if (is_connected) {
            ESP_LOGI(WIFI_LOG_TAG, "~~~~~~~~~~~");
            ESP_LOGI(WIFI_LOG_TAG, "IP:" IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(WIFI_LOG_TAG, "MASK:" IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(WIFI_LOG_TAG, "GW:" IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(WIFI_LOG_TAG, "RSSI: %ld", (long int)ap_info.rssi);
            ESP_LOGI(WIFI_LOG_TAG, "~~~~~~~~~~~");
            vTaskDelay(595000 / portTICK_PERIOD_MS);
        }
    }
}

// 1st stage initialization
esp_err_t wifi_manager_init(void) {
    ESP_ERROR_CHECK(esp_netif_init()); // Setup network stack
    
    // Event loop for handling system events (wifi connection / ip address acquision)
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Init wifi station interface
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Init WIFI using default config and init handlers for WIFI and IP events
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    
    return ESP_OK;
}

// Configure credientals and station mode
esp_err_t wifi_set_config(const char* ssid, const char* pass) {
    // Set the credientals
    wifi_config_t wifi_config = {};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password) - 1);
    ESP_LOGI(WIFI_LOG_TAG, "Set WiFi configuration SSID %s...", wifi_config.sta.ssid);

    // Set Station mode and configure SSID and PASS
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    return ESP_OK;
}

// Start the WIFI!
esp_err_t wifi_start(void) {
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}

#endif // WIFI_MANAGER_H