#include "signal_processor.h"

SignalProcessor::SignalProcessor() 
    : filterCutoff_(1000000), dcOffset_(0), 
      filterState_(0), filterCoefficient_(0.1) {}

float SignalProcessor::applyFilters(float rawValue) {
    // Simple low-pass IIR filter implementation
    filterState_ = (1.0 - filterCoefficient_) * filterState_ + 
                  filterCoefficient_ * rawValue;
    
    // Apply DC offset
    return filterState_ + dcOffset_;
}

void SignalProcessor::calibrateDCOffset() {
    // Placeholder for actual calibration routine
    // This would normally sample the output and calculate offset
    dcOffset_ = 0.05;  // Simulated calibration result
}

void SignalProcessor::setLowPassFilter(float cutoff) {
    filterCutoff_ = constrain(cutoff, 1000, 10000000);
    
    // Calculate coefficient based on cutoff frequency
    // Simple approximation: T = 1/(2πf), coefficient = 1 - e^(-1/(T*sampleRate))
    // Assuming sample rate of 1MHz for processing
    float timeConstant = 1.0 / (2 * PI * filterCutoff_);
    filterCoefficient_ = 1.0 - exp(-1.0 / (timeConstant * 1000000));
    filterCoefficient_ = constrain(filterCoefficient_, 0.001, 0.5);
}

String SignalProcessor::getStatus() {
    char buffer[60];
    if(filterCutoff_ < 1000000) {
        snprintf(buffer, sizeof(buffer), "LPF: %.2f kHz | DC: %.2f mV", 
                 filterCutoff_ / 1000, dcOffset_ * 1000);
    } else {
        snprintf(buffer, sizeof(buffer), "LPF: %.2f MHz | DC: %.2f mV", 
                 filterCutoff_ / 1000000, dcOffset_ * 1000);
    }
    return String(buffer);
}
