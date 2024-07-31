#ifndef HELTEC_UNOFFICIAL_H
#define HELTEC_UNOFFICIAL_H

//################################################################################//
// Define Logging Tags

const char* SETUP_LOG_TAG = "Setup";
const char* LOOP_LOG_TAG = "Loop";
const char* GARDEN_LOG_TAG = "Garden App";
const char* DISPLAY_LOG_TAG = "Display Manager";
const char* WIFI_LOG_TAG = "WIFI Manager";
const char* HTTP_LOG_TAG = "HTTP Manager";

//################################################################################//
// Includes

// STD
#include <algorithm>
#include <inttypes.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <queue>
#include <string>

// Drivers
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "driver/temperature_sensor.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"

// Esp Libraries
#include "esp_sleep.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_http_server.h"

// RTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"

//NVS
#include "nvs_flash.h"

//ADC
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"

// Heltec Dependencies
#include "ssd1306.h"
#include "RadioLib.h"
#include "HotButton.h"

// Custom Libraries
#include "wifi_manager.h"
#include "http_server.h"

//################################################################################//

// Stats
#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

// USB
#define USB_VID 0x303a
#define USB_PID 0x1001

// WIFI
#define WIFI_SSID "manny"
#define WIFI_PASS "manny123"

// Little Orange LED
#define LED_BUILTIN = SOC_GPIO_PIN_COUNT + 48;
#define LED_PIN GPIO_NUM_35
#define LED_FREQ 5000

// Useful Pin Functions
#define analogInputToDigitalPin(p)  (((p) < 20) ? (analogChannelToDigitalPin(p)) : -1)
#define digitalPinToInterrupt(p)    (((p) < 49) ? (p) : -1)
#define digitalPinHasPWM(p)         ((p) < 46)

#define HIGH 1
#define LOW 0

// Constants and GPIO definitions
#define PRG_BUTTON GPIO_NUM_0
HotButton button(PRG_BUTTON);
#define USB_VID 0x303a
#define USB_PID 0x1001

#define VEXT_CTRL GPIO_NUM_36
#define VBAT_READ GPIO_NUM_1
#define ADC_CTRL GPIO_NUM_37

// Display
#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH  128
#define SDA_OLED GPIO_NUM_17
#define SCL_OLED GPIO_NUM_18
#define RST_OLED GPIO_NUM_21
#define DIO0 GPIO_NUM_14;
#define DIO1 DIO0;

// LORA
#define RST_LoRa = GPIO_NUM_12;
#define BUSY_LoRa = GPIO_NUM_13;

// Touch Pins
#define TOUCH1 GPIO_NUM_1
#define TOUCH2 GPIO_NUM_2
#define TOUCH3 GPIO_NUM_3
#define TOUCH4 GPIO_NUM_4
#define TOUCH5 GPIO_NUM_5
#define TOUCH6 GPIO_NUM_6
#define TOUCH7 GPIO_NUM_7

// ADC1 Channels
#define ADC1_CH0 GPIO_NUM_1
#define ADC1_CH1 GPIO_NUM_2
#define ADC1_CH2 GPIO_NUM_3
#define ADC1_CH3 GPIO_NUM_4
#define ADC1_CH4 GPIO_NUM_5
#define ADC1_CH5 GPIO_NUM_6
#define ADC1_CH6 GPIO_NUM_7

// GPIO Pins
#define GPIO1 GPIO_NUM_1
#define GPIO2 GPIO_NUM_2
#define GPIO3 GPIO_NUM_3
#define GPIO4 GPIO_NUM_4
#define GPIO5 GPIO_NUM_5
#define GPIO6 GPIO_NUM_6
#define GPIO7 GPIO_NUM_7
#define GPIO19 GPIO_NUM_19
#define GPIO20 GPIO_NUM_20
#define GPIO21 GPIO_NUM_21
#define GPIO26 GPIO_NUM_26
#define GPIO33 GPIO_NUM_33
#define GPIO34 GPIO_NUM_34
#define GPIO35 GPIO_NUM_35
#define GPIO36 GPIO_NUM_36
#define GPIO37 GPIO_NUM_37
#define GPIO38 GPIO_NUM_38
#define GPIO39 GPIO_NUM_39
#define GPIO40 GPIO_NUM_40
#define GPIO41 GPIO_NUM_41
#define GPIO42 GPIO_NUM_42
#define GPIO43 GPIO_NUM_43
#define GPIO44 GPIO_NUM_44
#define GPIO45 GPIO_NUM_45
#define GPIO46 GPIO_NUM_46

// UART Pins
#define TX = GPIO_NUM_43;
#define RX = GPIO_NUM_44;

//################################################################################//
// External objects used in this driver

extern ssd1306_handle_t display;

//################################################################################//
// Function declarations

void heltec_setup();
void heltec_loop();
void heltec_deep_sleep(int seconds = 0);
void heltec_led(int percent);
float heltec_temperature();
bool heltec_wakeup_was_timer();

void init_nvs();
void init_display();
void init_wifi();
void init_webserver();

void display_centered_string(const char *str, uint8_t font_size, uint32_t duration_ms);
void display_stats();
void log_stats();

//################################################################################//
// Init functions

void init_nvs(){
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void init_display() {
    // Attach screen via I2C, log and return
    ESP_LOGI(SETUP_LOG_TAG, "Initializing I2C...");
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_OLED,
        .scl_io_num = SCL_OLED,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 400000
        },
        .clk_flags = 0
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));

    // Toggle reset if connected
    gpio_set_direction(RST_OLED, GPIO_MODE_OUTPUT);
    gpio_set_level(RST_OLED, 0); // Reset
    vTaskDelay(pdMS_TO_TICKS(100)); // Wait 100 ms
    gpio_set_level(RST_OLED, 1); // Out of reset

    // Initialize the display
    ESP_LOGI(DISPLAY_LOG_TAG, "Initializing SSD1306 display...");
    display = ssd1306_create(I2C_NUM_0, SSD1306_I2C_ADDRESS);
    if (ssd1306_init(display) == ESP_OK) {
        ESP_LOGI(DISPLAY_LOG_TAG, "Display initialized successfully");
        ssd1306_clear_screen(display, 0x00);
        ssd1306_refresh_gram(display);
    } else {
        ESP_LOGE(DISPLAY_LOG_TAG, "Failed to initialize display");
    }
}

void init_wifi() {
    ESP_ERROR_CHECK(wifi_manager_init());
    ESP_ERROR_CHECK(wifi_set_config(WIFI_SSID, WIFI_PASS));
    ESP_ERROR_CHECK(wifi_start());

    while (!wifi_manager_is_connected()) {
        ESP_LOGI(GARDEN_LOG_TAG, "Waiting for WiFi connection setup...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
} 

void init_webserver(){
    httpd_handle_t server = start_webserver();
    if (server == NULL) {
        ESP_LOGI(HTTP_LOG_TAG, "Failed to start web server!");
    } else {
        ESP_LOGI(HTTP_LOG_TAG, "Web server started!");
    }
}

//################################################################################//

// Helper Functions
void display_centered_string(const char *str, uint8_t font_size, uint32_t duration_ms) {
    // Function to display a centered string for a specific duration
    if (display == nullptr) {
        ESP_LOGE(SETUP_LOG_TAG, "Display handle is NULL");
        return;
    }

    // Clear the screen
    ssd1306_clear_screen(display, 0x00);
    ssd1306_refresh_gram(display);

    // Calculate string width and starting position to center the string
    // uint8_t str_width = font_size * strlen(str);
    // uint8_t start_x = (DISPLAY_WIDTH - str_width) / 2;
    uint8_t start_x = 0;
    uint8_t start_y = ((DISPLAY_HEIGHT - font_size) / 2) - font_size / 2;

    // Draw the string at the calculated position
    ssd1306_draw_string(display, start_x, start_y, (const uint8_t *)str, font_size, 1);

    // Refresh the display to show the string
    ssd1306_refresh_gram(display);

    // Keep the display on for the specified duration, otherwise
    if (duration_ms != 0){
        vTaskDelay(pdMS_TO_TICKS(duration_ms));
        // Clear the screen after the delay
        ssd1306_clear_screen(display, 0x00);
        ssd1306_refresh_gram(display);
    }

}

void display_stats() {
    char ip[16] = {0};
    wifi_manager_get_ip(ip);

    int32_t rssi = wifi_manager_get_rssi();
    float temperature = heltec_temperature();
    const int text_size = 12;
    // Create strings with variables
    std::string watering_status = "Water: Test";
    std::string connection_status = "Connected: " + std::to_string(wifi_manager_is_connected());
    std::string ip_status = std::string("IP: ") + ip;
    std::string rssi_status = "RSSI: " + std::to_string(rssi);
    std::string temperature_status = "Temp: " + std::to_string(temperature);

    ssd1306_clear_screen(display, 0x00);
    ssd1306_draw_string(display, 0, 0, (const uint8_t *)watering_status.c_str(), text_size, 1);
    ssd1306_draw_string(display, 0, 16, (const uint8_t *)connection_status.c_str(), text_size, 1);
    ssd1306_draw_string(display, 0, 32, (const uint8_t *)ip_status.c_str(), text_size, 1);
    ssd1306_draw_string(display, 0, 48, (const uint8_t *)rssi_status.c_str(), text_size, 1);
    ssd1306_draw_string(display, 0, 64, (const uint8_t *)temperature_status.c_str(), text_size, 1);
    ssd1306_refresh_gram(display);
}

void log_stats() {
    char ip[16] = {0};
    wifi_manager_get_ip(ip);

    int32_t rssi = wifi_manager_get_rssi();
    float temperature = heltec_temperature();

    // Create strings with variables
    std::string watering_status = "Watering Status: Test";
    std::string connection_status = "Connected: " + std::to_string(wifi_manager_is_connected());
    std::string ip_status = std::string("IP: ") + ip;
    std::string rssi_status = "RSSI: " + std::to_string(rssi);
    std::string temperature_status = "Temperature: " + std::to_string(temperature);

    // Log stats to the console
    ESP_LOGI(GARDEN_LOG_TAG, "-------------------------");
    ESP_LOGI(GARDEN_LOG_TAG, "%s", watering_status.c_str());
    ESP_LOGI(GARDEN_LOG_TAG, "%s", connection_status.c_str());
    ESP_LOGI(GARDEN_LOG_TAG, "%s", ip_status.c_str());
    ESP_LOGI(GARDEN_LOG_TAG, "%s", rssi_status.c_str());
    ESP_LOGI(GARDEN_LOG_TAG, "%s", temperature_status.c_str());
    ESP_LOGI(GARDEN_LOG_TAG, "-------------------------");
}

//################################################################################//

// Driver Functions
void heltec_setup() {
    esp_log_level_set(SETUP_LOG_TAG, ESP_LOG_INFO);
    ESP_LOGI(SETUP_LOG_TAG, "Starting setup");

    init_nvs();
    init_display();
    init_wifi();
    init_webserver();

    // ADC configuration for battery monitoring disabled for now
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten(VBAT_ADC, ADC_ATTEN_DB_11); // Adjust as necessary

    display_centered_string("ESP32 Setup Complete!", 12, 1000);
    ESP_LOGI(SETUP_LOG_TAG, "Setup complete");
}

void heltec_loop() {
    // Handle button press disabled for now
    int btn_level = gpio_get_level(PRG_BUTTON);
    if (btn_level == 0) {  // Assuming active-low button
        ESP_LOGI(LOOP_LOG_TAG, "Button pressed");
        display_centered_string("Button pressed", 12, 1000);
    }
}

void heltec_led(int percent) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = LED_FREQ,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LED_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = (uint32_t)((255 * percent) / 100),
        .hpoint = 0,
        .flags = {0},
    };
    ledc_channel_config(&ledc_channel);
}

float heltec_temperature() {
    esp_err_t ret;
    float temp = 0.0;
    temperature_sensor_handle_t temp_handle = NULL;
    temperature_sensor_config_t temp_config = {
        .range_min = -10,
        .range_max = 80,
    };

    ret = temperature_sensor_install(&temp_config, &temp_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(SETUP_LOG_TAG, "Failed to install temperature sensor");
        return temp;
    }

    ret = temperature_sensor_enable(temp_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(SETUP_LOG_TAG, "Failed to start temperature sensor");
        temperature_sensor_uninstall(temp_handle);
        return temp;
    }

    ret = temperature_sensor_get_celsius(temp_handle, &temp);
    if (ret != ESP_OK) {
        ESP_LOGE(SETUP_LOG_TAG, "Failed to read temperature");
    }

    temperature_sensor_disable(temp_handle);
    temperature_sensor_uninstall(temp_handle);
    return temp;
}


// Deep Sleep disabled for now
// void heltec_deep_sleep(int seconds) {
//     esp_sleep_enable_timer_wakeup(seconds * 1000000);
//     esp_deep_sleep_start();
// }
// bool heltec_wakeup_was_timer() {
//     return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
// }

//################################################################################//

#endif // HELTEC_UNOFFICIAL_H
