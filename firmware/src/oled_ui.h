#ifndef OLED_UI_H
#define OLED_UI_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class OLED_UI {
public:
    OLED_UI(uint8_t sda, uint8_t scl, int8_t rst = -1);
    bool begin();
    void update(float frequency, const String& waveform, const String& ip);
    void showSplashScreen();
    
private:
    Adafruit_SSD1306 display_;
    uint8_t sda_;
    uint8_t scl_;
};

#endif
