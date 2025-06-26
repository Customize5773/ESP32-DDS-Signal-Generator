import math

# Precomputed waveform tables (256 samples)
def generate_sine():
    return [int(128 + 127 * math.sin(2 * math.pi * i / 256)) for i in range(256)]

def generate_square():
    return [0 if i < 128 else 255 for i in range(256)]

def generate_sawtooth():
    return [i for i in range(256)]

def generate_triangle():
    return [i if i < 128 else 255 - (i - 128) for i in range(256)]

# Waveform dictionary
WAVEFORMS = {
    "sine": generate_sine(),
    "square": generate_square(),
    "sawtooth": generate_sawtooth(),
    "triangle": generate_triangle()
}

def get_waveform(name):
    return WAVEFORMS.get(name, WAVEFORMS["sine"])
