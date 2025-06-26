from dac_driver import DACDriver
from frequency_controller import FrequencyController
from dds_engine import DDSEngine
from machine import Pin
import time

# Initialize hardware
dac = DACDriver(pin=25)
led = Pin(2, Pin.OUT)  # Built-in LED

# Initialize DDS components
freq_ctrl = FrequencyController(sample_rate=40000)
dds = DDSEngine(dac, freq_ctrl, waveform="sine")

# Start DDS engine
dds.start()

# Main control loop
try:
    while True:
        # Blink LED to show activity
        led.value(not led.value())
        
        # Example: Cycle through frequencies
        for freq in [100, 500, 1000, 2000, 5000]:
            freq_ctrl.set_frequency(freq)
            time.sleep(2)
            
        # Example: Cycle through waveforms
        for wave in ["sine", "square", "sawtooth", "triangle"]:
            dds.set_waveform(wave)
            time.sleep(2)
            
except KeyboardInterrupt:
    dds.stop()
    print("DDS generator stopped")
