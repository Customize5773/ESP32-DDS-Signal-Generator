#ifndef AD9833_CONTROLLER_H
#define AD9833_CONTROLLER_H

#include <Arduino.h>
#include <SPI.h>

// AD9833 Register Bits
#define B28     13
#define HLB     12
#define FSELECT 11
#define PSELECT 10
#define RESET   8
#define SLEEP1  7
#define SLEEP12 6
#define OPBITEN 5
#define DIV2    3
#define MODE    1

// Waveform Definitions
#define AD9833_SINE     0
#define AD9833_SQUARE   (1 << OPBITEN)
#define AD9833_TRIANGLE (1 << MODE)

class AD9833Controller {
public:
    AD9833Controller(uint8_t fsyncPin);
    
    void begin();
    void reset();
    void setFrequency(float frequency, uint16_t waveform = AD9833_SINE);
    void sleep(bool enable = true);
    
private:
    void writeRegister(uint16_t data);
    uint8_t fsyncPin_;
    uint16_t controlReg_;
};

#endif
