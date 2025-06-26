#include "ad9833_controller.h"

AD9833Controller::AD9833Controller(uint8_t fsyncPin) 
    : fsyncPin_(fsyncPin), controlReg_(0) {}

void AD9833Controller::begin() {
    pinMode(fsyncPin_, OUTPUT);
    digitalWrite(fsyncPin_, HIGH);
    SPI.begin();
    reset();
}

void AD9833Controller::reset() {
    controlReg_ |= (1 << RESET);
    writeRegister(controlReg_);
    delayMicroseconds(10);
    controlReg_ &= ~(1 << RESET);
    writeRegister(controlReg_);
}

void AD9833Controller::setFrequency(float frequency, uint16_t waveform) {
    // Validate frequency input
    frequency = constrain(frequency, 0.01, 12500000);
    
    // Calculate frequency word
    uint32_t freqWord = static_cast<uint32_t>(frequency * 268435456.0 / 25000000.0 + 0.5);
    
    // Configure control register
    controlReg_ &= ~(0x3FF);  // Clear waveform bits
    controlReg_ |= waveform | (1 << B28);  // Enable 28-bit mode
    
    // Write to frequency register 0
    writeRegister(0x4000);  // FREQ0 write command
    writeRegister(freqWord & 0x3FFF);  // LSB
    writeRegister((freqWord >> 14) & 0x3FFF);  // MSB
    
    // Update control register
    writeRegister(controlReg_);
}

void AD9833Controller::sleep(bool enable) {
    controlReg_ = enable ? 
        controlReg_ | (1 << SLEEP12) : 
        controlReg_ & ~(1 << SLEEP12);
    writeRegister(controlReg_);
}

void AD9833Controller::writeRegister(uint16_t data) {
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE2));
    digitalWrite(fsyncPin_, LOW);
    SPI.transfer16(data);
    digitalWrite(fsyncPin_, HIGH);
    SPI.endTransaction();
}
