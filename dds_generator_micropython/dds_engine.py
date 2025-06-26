import machine
from waveforms import get_waveform

class DDSEngine:
    def __init__(self, dac, frequency_ctrl, waveform="sine"):
        self.dac = dac
        self.freq_ctrl = frequency_ctrl
        self.waveform = get_waveform(waveform)
        self.timer = machine.Timer(0)
        self._running = False
        
    def set_waveform(self, waveform):
        self.waveform = get_waveform(waveform)
        
    def start(self, sample_rate=None):
        if sample_rate:
            self.freq_ctrl.sample_rate = sample_rate
            self.freq_ctrl.update_increment()
            
        if not self._running:
            self.timer.init(
                period=max(1, int(1000 / self.freq_ctrl.sample_rate)),
                mode=machine.Timer.PERIODIC,
                callback=self._update
            )
            self._running = True
            
    def stop(self):
        if self._running:
            self.timer.deinit()
            self._running = False
            
    def _update(self, timer):
        phase_index = self.freq_ctrl.next_phase()
        self.dac.write(self.waveform[phase_index])
        
    def is_running(self):
        return self._running
