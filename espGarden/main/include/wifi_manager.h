#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_netif.h"

static const char *TAG = "wifi_manager";
static EventGroupHandle_t wifi_event_group;
static esp_netif_t *sta_netif = NULL;
const int CONNECTED_BIT = BIT0;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

esp_err_t wifi_manager_init(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    return ESP_OK;
}

esp_err_t wifi_manager_set_credentials(const char *ssid, const char *password) {
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "",
            .password = ""
        },
    };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    return ESP_OK;
}

esp_err_t wifi_manager_connect(void) {
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}

int32_t wifi_manager_get_rssi(void) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return ap_info.rssi;
    } else {
        // ESP_LOGE(TAG, "Failed to get AP info");
        return 0;  // Return 0 if unable to get RSSI
    }
}

esp_err_t wifi_manager_get_ip(char* ip){
    esp_netif_ip_info_t net_info;
    memset(&net_info, 0, sizeof(esp_netif_ip_info_t));
    if (esp_netif_get_ip_info(sta_netif, &net_info) == 0) 
        sprintf(ip, "" IPSTR, IP2STR(&net_info.ip));
    ESP_LOGE(TAG, "Getting IP");    
    return ESP_OK;
}

void wifi_task(void *pvParameters) {
    int32_t rssi;
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        rssi = wifi_manager_get_rssi();

        if (esp_netif_get_ip_info(sta_netif, &ip) == 0 && rssi != 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "MASK:" IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "RSSI: %ld", rssi);
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
    }
}

void wifi_init(const char *ssid, const char *password){
    ESP_ERROR_CHECK(wifi_manager_init());

    ESP_ERROR_CHECK(wifi_manager_set_credentials(ssid, password));

    ESP_ERROR_CHECK(wifi_manager_connect());

    // Create a task to monitor the connection status
    xTaskCreate(&wifi_task, "wifi_task", 4096, NULL, 5, NULL);
}
#endif // WIFI_MANAGER_H