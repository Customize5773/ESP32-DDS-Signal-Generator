#include "oled_ui.h"

OLED_UI::OLED_UI(uint8_t sda, uint8_t scl, int8_t rst) 
    : display_(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, rst),
      sda_(sda), scl_(scl) {}

bool OLED_UI::begin() {
    Wire.begin(sda_, scl_);
    if(!display_.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return false;
    }
    display_.clearDisplay();
    display_.setTextSize(1);
    display_.setTextColor(SSD1306_WHITE);
    display_.cp437(true);
    display_.display();
    return true;
}

void OLED_UI::update(float frequency, const String& waveform, const String& ip) {
    display_.clearDisplay();
    
    // Header
    display_.setCursor(0, 0);
    display_.print("DDS Signal Gen v1.0");
    
    // Frequency
    display_.setCursor(0, 20);
    display_.setTextSize(2);
    
    // Format frequency based on magnitude
    if(frequency < 1000) {
        display_.print(frequency, 2);
        display_.print(" Hz");
    } else if(frequency < 1000000) {
        display_.print(frequency / 1000, 2);
        display_.print(" kHz");
    } else {
        display_.print(frequency / 1000000, 2);
        display_.print(" MHz");
    }
    
    // Waveform
    display_.setTextSize(1);
    display_.setCursor(0, 45);
    display_.print("Wave: ");
    display_.print(waveform);
    
    // IP Address
    display_.setCursor(0, 56);
    display_.print("IP: ");
    display_.print(ip);
    
    display_.display();
}

void OLED_UI::showSplashScreen() {
    display_.clearDisplay();
    display_.setCursor(10, 15);
    display_.setTextSize(2);
    display_.print("DDS GEN");
    display_.setCursor(30, 40);
    display_.setTextSize(1);
    display_.print("Initializing...");
    display_.display();
}
