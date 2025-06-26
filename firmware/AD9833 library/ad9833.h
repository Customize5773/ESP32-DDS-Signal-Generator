#ifndef AD9833_H
#define AD9833_H

#include <SPI.h>
#include <Arduino.h>

// AD9833 Control Register Bits
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

class AD9833 {
public:
    AD9833(int fsyncPin);
    
    void begin();
    void reset();
    void setFrequency(float frequency, uint16_t waveform = AD9833_SINE);
    
private:
    int _fsyncPin;
    uint16_t _controlReg;
    
    void writeRegister(uint16_t data);
    void setFrequencyReg(uint16_t reg, float frequency);
};

#endif
