#include <M5Unified.h>
#include <driver/timer.h>
#include "optical_sensor.h"

constexpr int SENSOR_NUM = 4;
OpticalSensor opticalSensor[SENSOR_NUM] = {OpticalSensor(6), OpticalSensor(8), OpticalSensor(7), OpticalSensor(5)};
// OpticalSensor opticalSensor[SENSOR_NUM] = {OpticalSensor(5)};

void setup()
{
  M5.begin();
  M5.Display.setRotation(0);
  M5.Display.fillScreen(BLACK);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setFont(&fonts::FreeMonoBold12pt7b);

  Serial1.begin(115200, SERIAL_8N1, 38, 39); // EXT_IO rx, tx

  for (int i = 0; i < SENSOR_NUM; i++)
  {
    opticalSensor[i].init();
  }
}

void loop()
{
  char buf[10] = {0};
  uint64_t elapsed = 0;
  uint16_t rpm[SENSOR_NUM] = {0};

  for (int i = 0; i < SENSOR_NUM; i++)
  {
    double rpm_d = opticalSensor[i].getRpm();
    double hz = rpm_d / 60.0;
    rpm[i] = (uint16_t)rpm_d;

    M5.Display.fillRect(64 * (i % 2), 64 * (i / 2), 63, 63, TFT_BLACK);
    M5.Display.drawRect(0, 0, 128, 128, TFT_WHITE);
    M5.Display.drawLine(64, 0, 64, 127, TFT_WHITE);
    M5.Display.drawLine(0, 64, 127, 64, TFT_WHITE);

    M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
    String str;
    str = String(rpm[i]);
    M5.Display.drawCentreString(str, 32 + 64 * (i % 2), 12 + 64 * (i / 2));

    M5.Display.setTextColor(TFT_DARKGRAY, TFT_BLACK);
    // hzを小数点以下1桁まで表示
    str = String(hz, 1);
    M5.Display.drawCentreString(str, 32 + 64 * (i % 2), 40 + 64 * (i / 2));
  }

  memcpy(&buf[1], &rpm[1], 2);
  memcpy(&buf[3], &rpm[2], 2);
  memcpy(&buf[5], &rpm[0], 2);
  memcpy(&buf[7], &rpm[3], 2);

  buf[0] = 0xff;
  buf[9] = buf[1] + buf[2] + buf[3] + buf[4] + buf[5] + buf[6] + buf[7] + buf[8];
  Serial1.write(buf, 10);

  vTaskDelay(1000 / portTICK_RATE_MS);

  while (Serial1.available())
  {
    int inByte = Serial1.read();
    Serial.printf("%02x ", inByte);
  }
}