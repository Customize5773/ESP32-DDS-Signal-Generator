from machine import DAC, Pin

class DACDriver:
    def __init__(self, pin=25):
        self.dac = DAC(Pin(pin))
        self.current_value = 0
        self.range = (0, 255)  # 8-bit DAC range
        
    def write(self, value):
        """Write value to DAC with range checking"""
        value = max(self.range[0], min(self.range[1], int(value)))
        self.dac.write(value)
        self.current_value = value
        
    def set_amplitude(self, amplitude):
        """Scale output amplitude (0.0 to 1.0)"""
        self.range = (0, int(255 * max(0.0, min(1.0, amplitude)))
        
    def get_value(self):
        return self.current_value
