class FrequencyController:
    def __init__(self, sample_rate=40000):
        self.sample_rate = sample_rate
        self.frequency = 1000  # Default 1kHz
        self.phase_accumulator = 0
        self.phase_increment = 0
        self.update_increment()
        
    def set_frequency(self, freq):
        self.frequency = max(0.1, min(20000, freq))  # Limit to 20kHz for audio
        self.update_increment()
        
    def update_increment(self):
        """Calculate phase increment based on current frequency"""
        self.phase_increment = int((self.frequency * 65536) / self.sample_rate)
        
    def next_phase(self):
        """Get next phase value and update accumulator"""
        self.phase_accumulator = (self.phase_accumulator + self.phase_increment) & 0xFFFF
        return self.phase_accumulator >> 8  # Return upper 8 bits for 256-entry table
