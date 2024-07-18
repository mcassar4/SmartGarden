#ifndef HOT_BUTTON_H
#define HOT_BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include <algorithm>

#define MAX_PRESSES 4
#define DEBOUNCE_TIME 20
#define EVENT_TIMEOUT 250

#define BETWEEN(a, b)   ((((a / 10) & 0xFF) << 8) | ((b / 10) & 0xFF))
#define OVER(a)         ((((a / 10) & 0xFF) << 8) | 0xFF)
#define UNDER(b)        ((b / 10) & 0xFF)

#define DIT             UNDER(200)
#define SHORT           DIT
#define DA              OVER(200)
#define LONG            DA

#define HIGH 1
#define LOW 0

#define SINGLECLICK     DIT
#define DOUBLECLICK     DIT, DIT
#define TRIPLECLICK     DIT, DIT, DIT
#define QUADRUPLECLICK  DIT, DIT, DIT, DIT

class HotButton {
public:
    HotButton(int _pin, bool _pullup = false, bool _active = LOW) {
        pin = _pin;
        pullup = _pullup;
        active = _active;
        gpio_reset_pin((gpio_num_t)pin);
        gpio_set_direction((gpio_num_t)pin, GPIO_MODE_INPUT);
        if (pullup) {
            gpio_set_pull_mode((gpio_num_t)pin, GPIO_PULLUP_ONLY);
        }
    }

    void update() {
        // Deaf for DEBOUNCE_TIME ms after press or release
        if ((esp_timer_get_time() / 1000) - lastChange < DEBOUNCE_TIME) return;
        // If released for EVENT_TIMEOUT, go idle
        if (!curState && (esp_timer_get_time() / 1000) - lastChange > EVENT_TIMEOUT && !idle) idle = true;
        // Inverting and XORing active: state true when button pressed
        bool state = !gpio_get_level((gpio_num_t)pin) ^ active;
        // Same as last time, nothing to do
        if (state == curState) return;
        // State has changed
        if (state) {
            // Button pressed, forget previous event clicks if idle in-between
            if (idle) pressCount = 0;
        } else if (pressCount < MAX_PRESSES) {
            // Button released, store button press in ms / 10
            presses[pressCount++] = std::min(255, (int)(((esp_timer_get_time() / 1000) - lastChange) / 10));
            lastPressedFor = 0;
        }
        idle = false;
        curState = state;
        lastChange = esp_timer_get_time() / 1000; 
    }

    bool event(uint16_t click1) {
        if (idle && pressCount == 1 && isBetween(click1, presses[0] * 10)) {
            pressCount = 0;
            return true;
        }
        return false;
    }

    bool event(uint16_t click1, uint16_t click2) {
        if (idle && pressCount == 2 && isBetween(click1, presses[0] * 10) && isBetween(click2, presses[1] * 10)) {
            pressCount = 0;
            return true;          
        }
        return false;
    }

    bool event(uint16_t click1, uint16_t click2, uint16_t click3) {
        if (idle && pressCount == 3 && isBetween(click1, presses[0] * 10) && isBetween(click2, presses[1] * 10) && isBetween(click3, presses[2] * 10)) {
            pressCount = 0;
            return true;          
        }
        return false;
    }

    bool event(uint16_t click1, uint16_t click2, uint16_t click3, uint16_t click4) {
        if (idle && pressCount == 4 && isBetween(click1, presses[0] * 10) && isBetween(click2, presses[1] * 10) && isBetween(click3, presses[2] * 10) && isBetween(click4, presses[3] * 10)) {
            pressCount = 0;
            return true;          
        }
        return false;
    }

    bool isSingleClick() {
        return event(SINGLECLICK);
    }

    bool isDoubleClick() {
        return event(DOUBLECLICK);
    }

    bool isTripleClick() {
        return event(TRIPLECLICK);
    }

    bool isQuadrupleClick() {
        return event(QUADRUPLECLICK);
    }

    bool pressedFor(int time) {
        if (curState && (esp_timer_get_time() / 1000) - lastChange > time && time > lastPressedFor) {
            lastPressedFor = time;
            return true;
        }
        return false;
    }

    bool pressed() {
        return pressedFor(1);
    }

    bool pressedNow() {
        return gpio_get_level((gpio_num_t)pin) == active;
    }

    void waitForPress() {
        while (gpio_get_level((gpio_num_t)pin) != active) vTaskDelay(10 / portTICK_PERIOD_MS);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

    void waitForRelease() {
        while (gpio_get_level((gpio_num_t)pin) == active) vTaskDelay(10 / portTICK_PERIOD_MS);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }

private:
    static bool isBetween(const uint16_t betweenValue, int time) {
        int from = (betweenValue >> 8) * 10;
        int to = (betweenValue & 0xFF) * 10;
        return time > from && time <= to;
    }

    uint8_t pin;
    bool pullup;
    bool active;
    bool curState = false;
    bool idle = true;
    uint8_t presses[MAX_PRESSES];
    uint8_t pressCount = 0;
    uint64_t lastChange = 0;
    uint16_t lastPressedFor = 0;
};

#endif // HOT_BUTTON_H
