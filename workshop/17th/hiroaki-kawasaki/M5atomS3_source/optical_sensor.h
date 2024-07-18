#pragma once

#include <M5Unified.h>
#include <driver/timer.h>
#include <esp_timer.h>

constexpr uint32_t OPTICAL_INTERVAL_US = 200 * 1000;
constexpr int BUFFER_SIZE = 64;
constexpr double HZ_RATIO = (1000000.0 / OPTICAL_INTERVAL_US) / BUFFER_SIZE;

// optical_sensorのクラス
class OpticalSensor
{
public:
    OpticalSensor(int pin);

    void init();
    double getRpm();

private:
    static void IRAM_ATTR optical_cb(void *arg);

    int pin_OPTICAL;

    uint16_t _count = 0;
    uint64_t _start_time = 0;
    uint64_t _latest_time = 0;
};
