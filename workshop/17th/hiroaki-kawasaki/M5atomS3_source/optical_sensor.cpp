#include <driver/timer.h>
#include <esp_timer.h>

// #define SENSOR_CHECK

#include "optical_sensor.h"

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;
#define _ENTER_CRITICAL() portENTER_CRITICAL_SAFE(&spinlock)
#define _EXIT_CRITICAL() portEXIT_CRITICAL_SAFE(&spinlock)

#ifdef SENSOR_CHECK
int test_output_pin = 6;
#endif

OpticalSensor::OpticalSensor(int pin) : pin_OPTICAL(pin),
                                        _count(0),
                                        _start_time(0),
                                        _latest_time(0)
{
}

void OpticalSensor::init()
{
    attachInterruptArg(pin_OPTICAL, optical_cb, this, FALLING); // INT0

#ifdef SENSOR_CHECK
    pinMode(test_output_pin, OUTPUT);
#endif
}

void OpticalSensor::optical_cb(void *arg)
{
    OpticalSensor *opticalSensor = (OpticalSensor *)arg;

    _ENTER_CRITICAL();

    uint64_t now = esp_timer_get_time();
    opticalSensor->_latest_time = now;

    if (opticalSensor->_start_time == 0)
    {
        opticalSensor->_start_time = now;
    }

    opticalSensor->_count++;

    _EXIT_CRITICAL();

#ifdef SENSOR_CHECK
    // トグルする
    digitalWrite(test_output_pin, !digitalRead(test_output_pin));
#endif
}

double OpticalSensor::getRpm()
{
    _ENTER_CRITICAL();

    uint64_t elapsed = _latest_time - _start_time;
    _start_time = 0;

    uint16_t count = _count;
    _count = 0;

    _EXIT_CRITICAL();

    if (count <= 1 || elapsed <= 0)
    {
        return 0.0;
    }

    double mean_time = elapsed / (count - 1);
    double rpm = 60.0 * 1000000.0 / mean_time;

    return rpm;
}