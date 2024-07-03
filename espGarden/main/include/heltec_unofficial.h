#ifndef HELTEC_UNOFFICIAL_H
#define HELTEC_UNOFFICIAL_H

#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/ledc.h"
#include "driver/temp_sensor.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/i2c_master.h"
#include "sdkconfig.h"

#include "ssd1306.h"
#include "RadioLib.h"

#define USB_VID 0x303a
#define USB_PID 0x1001

#define EXTERNAL_NUM_INTERRUPTS 46
#define NUM_DIGITAL_PINS        48
#define NUM_ANALOG_INPUTS       20

static const uint8_t LED_BUILTIN = SOC_GPIO_PIN_COUNT+48;
#define BUILTIN_LED  LED_BUILTIN // backward compatibility
#define LED_BUILTIN LED_BUILTIN
#define RGB_BUILTIN LED_BUILTIN
#define RGB_BRIGHTNESS 64

#define analogInputToDigitalPin(p)  (((p)<20)?(analogChannelToDigitalPin(p)):-1)
#define digitalPinToInterrupt(p)    (((p)<49)?(p):-1)
#define digitalPinHasPWM(p)         (p < 46)

#define HIGH 1
#define LOW 0
#define BUTTON GPIO_NUM_0

// Constants and GPIO definitions
#define LED_PIN GPIO_NUM_35
#define LED_FREQ 5000
#define LED_CHAN 0
#define LED_RES 8

#define VEXT GPIO_NUM_36
#define VBAT_CTRL GPIO_NUM_37
#define VBAT_ADC ADC1_CHANNEL_0

#define SS GPIO_NUM_8
#define SCK GPIO_NUM_9
#define MOSI GPIO_NUM_10
#define MISO GPIO_NUM_11

#define DIO1 GPIO_NUM_14
#define RST_LoRa GPIO_NUM_12
#define BUSY_LoRa GPIO_NUM_13

#define SDA_OLED GPIO_NUM_17
#define SCL_OLED GPIO_NUM_18
#define RST_OLED GPIO_NUM_21

#define A0 = 1;
#define A1 = 2;
#define A2 = 3;
#define A3 = 4;
#define A4 = 5;
#define A5 = 6;
#define A6 = 7;
#define A7 = 8;
#define A8 = 9;
#define A9 = 10;
#define A10 = 11;
#define A11 = 12;
#define A12 = 13;
#define A13 = 14;
#define A14 = 15;
#define A15 = 16;
#define A16 = 17;
#define A17 = 18;
#define A18 = 19;
#define A19 = 20;
#define T1 = 1;
#define T2 = 2;
#define T3 = 3;
#define T4 = 4;
#define T5 = 5;
#define T6 = 6;
#define T7 = 7;
#define T8 = 8;
#define T9 = 9;
#define T10 = 10;
#define T11 = 11;
#define T12 = 12;
#define T13 = 13;
#define T14 = 14;


// static const uint8_t TX = 43;
// static const uint8_t RX = 44;

// Battery voltage scale constants
const float min_voltage = 3.04;
const float max_voltage = 4.26;
const uint8_t scaled_voltage[100] = {
    254, 242, 230, 227, 223, 219, 215, 213, 210, 207,
    206, 202, 202, 200, 200, 199, 198, 198, 196, 196,
    195, 195, 194, 192, 191, 188, 187, 185, 185, 185,
    183, 182, 180, 179, 178, 175, 175, 174, 172, 171,
    170, 169, 168, 166, 166, 165, 165, 164, 161, 161,
    159, 158, 158, 157, 156, 155, 151, 148, 147, 145,
    143, 142, 140, 140, 136, 132, 130, 130, 129, 126,
    125, 124, 121, 120, 118, 116, 115, 114, 112, 112,
    110, 110, 108, 106, 106, 104, 102, 101, 99, 97,
    94, 90, 81, 80, 76, 73, 66, 52, 32, 7
};

// Function prototypes
void heltec_setup();
void heltec_loop();
void heltec_deep_sleep(int seconds = 0);
void heltec_led(int percent);
void heltec_ve(bool state);
float heltec_vbat();
int heltec_battery_percent(float vbat = -1);
bool heltec_wakeup_was_timer();
float heltec_temperature();
void heltec_display_power(bool on);

// Global handle for the SSD1306
extern ssd1306_handle_t ssd1306_dev;
extern SX1276 radio; // Defined in main.cpp

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_OLED,
        .scl_io_num = SCL_OLED,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 400000
        }
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0));
}


void heltec_setup() {
    static const char* LOG_TAG = "HeltecSetup";
    esp_log_level_set(LOG_TAG, ESP_LOG_INFO);

    ESP_LOGI(LOG_TAG, "Initializing I2C...");
    i2c_master_init();

    // Toggle reset if connected
    gpio_set_direction(RST_OLED, GPIO_MODE_OUTPUT);
    gpio_set_level(RST_OLED, 0); // Reset
    vTaskDelay(pdMS_TO_TICKS(100)); // Wait 100 ms
    gpio_set_level(RST_OLED, 1); // Out of reset

    // Initialize the display
    ESP_LOGI(LOG_TAG, "Initializing SSD1306 display...");
    ssd1306_dev = ssd1306_create(I2C_NUM_0, SSD1306_I2C_ADDRESS); // Check your I2C configuration
    if (ssd1306_init(ssd1306_dev) == ESP_OK) {
        ESP_LOGI(LOG_TAG, "Display initialized successfully");
    } else {
        ESP_LOGE(LOG_TAG, "Failed to initialize display");
    }

    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t*)"Heltec ESP32 LoRa", 12, 1);
    ssd1306_refresh_gram(ssd1306_dev);

    // ADC configuration for battery monitoring
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(VBAT_ADC, ADC_ATTEN_DB_11); // Adjust as necessary

    ESP_LOGI(LOG_TAG, "Setup complete");
}

void heltec_loop() {
    static const char* LOG_TAG = "HeltecLoop";
    ESP_LOGI(LOG_TAG, "Loop running");

    // Handle button press for power management
    int btn_level = gpio_get_level(BUTTON);
    if (btn_level == 0) {  // Assuming active-low button
        ESP_LOGI(LOG_TAG, "Button pressed, entering deep sleep");
        heltec_deep_sleep(10); // Sleep for 10 seconds as an example
    }

    // Update display
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, 0, 16, (const uint8_t*)"Running loop...", 12, 1);
    ssd1306_refresh_gram(ssd1306_dev);

    // Delay in loop for readability in logs
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void heltec_deep_sleep(int seconds) {
    esp_sleep_enable_timer_wakeup(seconds * 1000000);
    esp_deep_sleep_start();
}

void heltec_led(int percent) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = LED_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num = LED_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = (uint32_t)((255 * percent) / 100)
    };
    ledc_channel_config(&ledc_channel);
}

void heltec_ve(bool state) {
    gpio_set_direction(VEXT, GPIO_MODE_OUTPUT);
    gpio_set_level(VEXT, state ? LOW : HIGH);
}

float heltec_vbat() {
    gpio_set_direction(VBAT_CTRL, GPIO_MODE_OUTPUT);
    gpio_set_level(VBAT_CTRL, LOW);
    vTaskDelay(pdMS_TO_TICKS(10));
    int raw = adc1_get_raw(VBAT_ADC);
    gpio_set_direction(VBAT_CTRL, GPIO_MODE_INPUT);
    return (raw / 4095.0) * 3.3;  // Assuming a max of 3.3V for ADC
}

int heltec_battery_percent(float vbat) {
    if (vbat == -1) {
        vbat = heltec_vbat();
    }
    int percentage = 0;
    for (int i = 0; i < 100; i++) {
        if (vbat >= min_voltage + (max_voltage - min_voltage) * (scaled_voltage[i] / 255.0)) {
            percentage = 100 - i;
            break;
        }
    }
    return percentage;
}

float heltec_temperature() {
    float temp;
    temp_sensor_read_celsius(&temp);
    return temp;
}

bool heltec_wakeup_was_timer() {
    return esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER;
}



#endif // HELTEC_UNOFFICIAL_H
