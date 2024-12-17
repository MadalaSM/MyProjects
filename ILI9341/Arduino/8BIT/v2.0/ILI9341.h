#ifndef ILI9341_h
#define ILI9341_h

#include <Arduino.h>

class ILI9341{
  public:
    //Constructor
    ILI9341(uint8_t RST, uint8_t CS, uint8_t RS, uint8_t SDA, uint8_t SCL, uint8_t LED);
    void init();
    void Address_set(int16_t y1, int16_t y2, int16_t x1, int16_t x2);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void writeData(uint8_t value);
    void dataWrite(uint8_t message);
    void commandWrite(uint8_t message);
  private:
    uint8_t _RST, _CS, _RS, _SDA, _SCL, _LED;
};

#endif