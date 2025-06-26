# ESP32-DDS-Signal-Generator


## Overview
The ESP32 "Advanced" DDS Signal Generator is a precision waveform generator built around the ESP32 microcontroller. It implements Direct Digital Synthesis (DDS) techniques to produce stable, configurable waveforms with professional-grade features. This project combines hardware design with robust software architecture to create a versatile instrument suitable for electronics testing, education, and embedded systems development.

## Key Features
- **Frequency Range**: 0.01Hz to 5MHz with 0.61Hz resolution at 40kHz sample rate
- **Waveforms**: Sine, Square, Triangle, Sawtooth (256-point precision)
- **Control Interfaces**:
  - Rotary encoder with push-button for local control
  - Web-based interface via WiFi AP (192.168.4.1)
  - OLED display for real-time parameter monitoring
- **Signal Quality**:
  - Low-pass filtering with configurable cutoff frequency
  - DC offset calibration
  - Output buffering with LM358 op-amp
- **Professional Architecture**:
  - Modular firmware design with separation of concerns
  - Hardware abstraction layer
  - Timer-driven sampling for precise timing
  - Error handling and input validation

## Hardware Requirements
| Component | Specification | Notes |
|-----------|--------------|-------|
| ESP32 | ESP32-WROOM-32 | DevKitC-V4 recommended |
| DDS Module | AD9833 | With 25MHz crystal |
| Display | 0.96" OLED | I2C SSD1306 128x64 |
| Rotary Encoder | EC11 | With push-button |
| Op-Amp | LM358 | For output buffering |
| Voltage Regulator | LM1117-3.3 | Clean power for analog section |
| Passive Components | Capacitors, Resistors | See BOM for details |

## Setup Instructions

### 1. Hardware Assembly
1. Connect components according to the schematic in `/documentation`
2. Ensure proper power filtering near analog components
3. Verify all connections with multimeter before powering

### 2. Software Installation
```bash
# Clone repository
git clone https://github.com/<your-username>/ESP32-Advanced-DDS-Signal-Generator.git

# Install required tools
pip install esptool adafruit-ampy

# Deploy firmware
ampy --port /dev/ttyUSB0 put firmware
```

### 3. Initial Configuration
1. Power on the device
2. Connect to WiFi AP: `DDS_Generator` (password: `generate123`)
3. Access web interface at `192.168.4.1`
4. Verify 1kHz sine wave output with oscilloscope

## Usage Examples

### Basic Operation
```python
from dds_engine import DDSEngine
from frequency_controller import FrequencyController
from dac_driver import DACDriver

# Initialize components
dac = DACDriver(pin=25)
freq_ctrl = FrequencyController(sample_rate=40000)
dds = DDSEngine(dac, freq_ctrl)

# Start output
dds.start()
freq_ctrl.set_frequency(1000)  # 1kHz
```

### Changing Waveforms
```python
# Available waveforms: 'sine', 'square', 'triangle', 'sawtooth'
dds.set_waveform('triangle')
```

### Frequency Sweep
```python
import time

for freq in range(100, 5001, 100):  # 100Hz to 5kHz in 100Hz steps
    freq_ctrl.set_frequency(freq)
    time.sleep(0.5)  # Dwell at each frequency
```

### Web Interface Controls
```html
<!-- Example web control -->
<button onclick="fetch('/set?freq=440')">440Hz Tone</button>
<button onclick="fetch('/wave?type=square')">Square Wave</button>
```

## Performance Specifications
| Parameter | Value | Notes |
|-----------|-------|-------|
| Frequency Range | 0.01Hz - 5MHz | Limited by ESP32 processing |
| Frequency Resolution | 0.61Hz | At 40kHz sample rate |
| Amplitude Range | 0-3.3V | Configurable via output stage |
| THD (1kHz sine) | <1% | With proper filtering |
| Phase Noise | -70dBc/Hz @ 1kHz offset | At 1MHz output |
| Sample Rate | Up to 40kHz | MicroPython limitation |

## Calibration
Refer to `/docs/calibration_procedure.md` for:
- Frequency accuracy calibration
- DC offset compensation
- Amplitude flatness correction
- Temperature compensation (advanced)

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements
- ESP32 hardware abstraction based on MicroPython
- DDS algorithm inspiration from Analog Devices AD9833 documentation
- Web interface using MicroPython's AsyncWebServer implementation
