# Hardware Setup Guide

## Required Tools
- Soldering iron & solder
- Wire cutters/strippers
- Multimeter
- Oscilloscope (recommended for verification)
- USB-C cable (for programming/power)

## Step 1: Prepare Components
| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 Dev Board | 1 | Use DevKitC V4 or equivalent |
| AD9833 Module | 1 | Verify 25MHz crystal version |
| OLED Display | 1 | 0.96" I2C SSD1306 |
| Rotary Encoder | 1 | EC11 type with switch |
| Breadboard | 1 | 830 points minimum |
| Jumper Wires | 40 | M/M, M/F combinations |
| LM1117-3.3 | 1 | Voltage regulator |
| LM358 | 1 | Op-amp for buffering |
| Ceramic Capacitors | 5 | 100nF (0805 package) |
| Resistors | 3 | 10Ω (2), 100Ω (1) |

## Step 2: Circuit Assembly

![Logic_Analyzer_on_ESP32](https://github.com/user-attachments/assets/625c3aa5-035d-4d08-b07c-89969b6ad410)
> by [AZDelivery](https://www.instructables.com/member/AZDelivery/)

### Power Connections:
1. Connect USB 5V to LM1117-3.3 input
2. Connect LM1117-3.3 output to AD9833 VCC
3. Add 100nF capacitor between VCC and GND near:
   - ESP32
   - AD9833
   - LM1117 input/output

### AD9833 Connections:
| AD9833 Pin | ESP32 Pin |
|------------|-----------|
| FSYNC      | GPIO5     |
| SCK        | GPIO18    |
| SDATA      | GPIO23    |
| VCC        | 3.3V Reg  |
| GND        | GND       |

### OLED Connections:
| OLED Pin | ESP32 Pin |
|----------|-----------|
| SDA      | GPIO21    |
| SCL      | GPIO22    |
| VCC      | 3.3V      |
| GND      | GND       |

### Rotary Encoder:
| Encoder Pin | ESP32 Pin |
|-------------|-----------|
| CLK         | GPIO16    |
| DT          | GPIO17    |
| SW          | GPIO4     |
| +           | 3.3V      |
| GND         | GND       |

### Output Stage:
```
AD9833 OUT → 10kΩ Resistor → LM358 Non-Inverting Input
                        ↓
                    100nF Cap → GND
                        ↓
LM358 Output → 100Ω Resistor → Output Jack
```

## Step 3: Software Installation
1. Install [Visual Studio Code](https://code.visualstudio.com/)
2. Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
3. Clone repository:
   ```bash
   git clone https://github.com/Customize5773/ESP32-Advanced-DDS-Signal-Generator.git
   ```
4. Open project in VSCode

## Step 4: Upload Firmware
1. Connect ESP32 via USB
2. In PlatformIO:
   - Select `esp32dev` environment
   - Click "Build" (checkmark icon)
   - Click "Upload" (right arrow icon)
3. Monitor serial output:
   ```bash
   pio device monitor
   ```

## Step 5: First Test
1. Power on the system
2. Verify OLED shows:
   ```
   DDS Signal Gen v1.0
   1000.00 Hz
   Wave: Sine
   IP: 192.168.4.1
   ```
3. Connect oscilloscope to output jack
4. Verify 1kHz sine wave output
5. Rotate encoder - frequency should change
6. Press encoder button - waveform should cycle

## Troubleshooting
| Symptom | Solution |
|---------|----------|
| No OLED display | Check I2C connections, try different address (0x3C or 0x3D) |
| No signal output | Verify AD9833 SPI connections, check 25MHz crystal |
| WiFi not appearing | Confirm ESP32 WiFi support, check power supply |
| Unstable frequency | Add 10μF capacitor to 3.3V line near AD9833 |

> **Warning:** Always double-check power connections before applying power to avoid component damage!
