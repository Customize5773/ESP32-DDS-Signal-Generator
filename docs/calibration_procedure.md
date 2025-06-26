# Calibration Guide

## Required Equipment
- Precision frequency counter (or oscilloscope with measurement)
- True RMS multimeter
- 50Ω terminator
- Variable power supply (optional)

## Frequency Calibration
1. Connect frequency counter to output jack
2. Set output to 1MHz sine wave via web interface
3. Measure actual frequency (F_measured)
4. Calculate correction factor:
   ```cpp
   correction = (1000000 * 268435456) / (F_measured * 25000000)
   ```
5. Modify `ad9833_controller.cpp`:
   ```cpp
   // Replace line 28:
   uint32_t freqWord = static_cast<uint32_t>(frequency * 268435456.0 / 25000000.0 + 0.5);
   
   // With calibrated version:
   uint32_t freqWord = static_cast<uint32_t>(frequency * correction * 268435456.0 / 25000000.0 + 0.5);
   ```
6. Reflash firmware and verify accuracy at multiple frequencies

## DC Offset Calibration
1. Connect multimeter in DC voltage mode to output
2. Set output to minimum amplitude
3. Measure DC offset voltage
4. In `signal_processor.cpp`, adjust:
   ```cpp
   void SignalProcessor::calibrateDCOffset() {
     // Change offset value (in volts)
     dcOffset_ = -0.0032; // Compensate measured offset
   }
   ```
5. Call calibration in setup:
   ```cpp
   // Add to setup() in main sketch:
   signalProcessor.calibrateDCOffset();
   ```

## Amplitude Flatness Calibration
| Frequency | Set Amplitude | Measured Amplitude | Correction Factor |
|-----------|--------------|-------------------|-------------------|
| 100 Hz    | 1.0 Vpp      | 0.98 Vpp          | 1.02              |
| 1 kHz     | 1.0 Vpp      | 1.00 Vpp          | 1.00              |
| 100 kHz   | 1.0 Vpp      | 0.95 Vpp          | 1.053             |
| 1 MHz     | 1.0 Vpp      | 0.85 Vpp          | 1.176             |

1. Create calibration table in `signal_processor.cpp`:
   ```cpp
   const float AMP_CORRECTION[][2] = {
     {100, 1.02},
     {1000, 1.00},
     {100000, 1.053},
     {1000000, 1.176}
   };
   ```
2. Implement correction in output processing:
   ```cpp
   float getAmplitudeCorrection(float frequency) {
     for(int i=0; i<4; i++) {
       if(frequency <= AMP_CORRECTION[i][0]) {
         return AMP_CORRECTION[i][1];
       }
     }
     return 1.176; // Default for >1MHz
   }
   ```

## Output Impedance Matching
1. Connect 50Ω terminator to output
2. Measure voltage drop at 1kHz:
   ```
   V_open = Voltage without load
   V_load = Voltage with 50Ω load
   ```
3. Calculate output impedance:
   ```
   Z_out = 50 * (V_open - V_load) / V_load
   ```
4. Add series resistor if Z_out > 1Ω:
   - Start with 10Ω resistor in output path
   - Re-measure and adjust until Z_out ≈ 50Ω

## Temperature Compensation (Advanced)
1. Place unit in temperature chamber
2. Measure frequency drift at -10°C, 25°C, 60°C
3. Implement compensation in software:
   ```cpp
   float tempCompensation(float temp) {
     // PPM/°C correction (example: -0.2ppm/°C)
     return 1.0 + (25.0 - temp) * -0.2e-6;
   }
   ```
4. Add temperature sensor (DS18B20) to circuit

## Verification Procedure
1. Frequency accuracy:
   - Test points: 10Hz, 1kHz, 100kHz, 1MHz, 5MHz
   - Tolerance: ±10ppm at 25°C
   
2. Amplitude flatness:
   - 20Hz-2MHz: ±1dB
   - 2MHz-5MHz: ±3dB

3. THD Measurement:
   | Frequency | Max THD |
   |-----------|--------|
   | 20Hz-20kHz | <1%    |
   | 20kHz-1MHz | <3%    |
   | >1MHz     | <5%    |

4. Save calibration report (Optional):
   ```
   /docs/calibration_reports/cal_v1.0_<date>.pdf
   ```

> **Note:** Recalibrate every 12 months or after hardware modifications
