#include "ad9833.h"

AD9833::AD9833(int fsyncPin) : _fsyncPin(fsyncPin) {}

void AD9833::begin() {
    pinMode(_fsyncPin, OUTPUT);
    digitalWrite(_fsyncPin, HIGH);
    SPI.begin();
    _controlReg = 0;
}

void AD9833::reset() {
    // Set reset bit and clear it after 10ns (immediate in code)
    _controlReg |= (1 << RESET);
    writeRegister(_controlReg);
    delayMicroseconds(10);
    _controlReg &= ~(1 << RESET);
    writeRegister(_controlReg);
}

void AD9833::setFrequency(float frequency, uint16_t waveform) {
    // Validate frequency input
    if (frequency < 0.01) frequency = 0.01;
    if (frequency > 12500000) frequency = 12500000;  // AD9833 max clock
    
    // Calculate frequency register value
    uint32_t freqWord = static_cast<uint32_t>(frequency * 268435456.0 / 25000000.0 + 0.5);
    
    // Configure control register
    _controlReg &= ~(0x3FF);  // Clear waveform bits
    _controlReg |= waveform | (1 << B28);  // Always use 28-bit mode
    
    // Write to frequency register 0
    writeRegister(0x4000);  // FREQ0 write, B28 sequence
    writeRegister(static_cast<uint16_t>(freqWord & 0x3FFF));
    writeRegister(static_cast<uint16_t>((freqWord >> 14) & 0x3FFF));
    
    // Update control register
    writeRegister(_controlReg);
}

// Private method to write to AD9833 register
void AD9833::writeRegister(uint16_t data) {
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE2));
    digitalWrite(_fsyncPin, LOW);
    SPI.write16(data);
    digitalWrite(_fsyncPin, HIGH);
    SPI.endTransaction();
}
