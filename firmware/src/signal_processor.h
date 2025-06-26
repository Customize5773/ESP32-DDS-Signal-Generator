#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <Arduino.h>

class SignalProcessor {
public:
    SignalProcessor();
    float applyFilters(float rawValue);
    void calibrateDCOffset();
    void setLowPassFilter(float cutoff);
    String getStatus();
    
private:
    float filterCutoff_;
    float dcOffset_;
    float filterState_;
    float filterCoefficient_;
};

#endif
