# /// script
# dependencies = [ "matplotlib", "numpy", "scipy" ]
# ///

# https://www.dsprelated.com/showarticle/1113.php

import numpy as np

from matplotlib import pyplot as plot
from scipy.signal import freqz
from scipy.signal.windows import kaiser, hann

taps = 19
assert taps % 2

N = taps - 1
n = np.arange(-N//2, N//2 + 1)
assert len(n) == taps

win = kaiser(taps, 6) if True else hann(taps)
eps = np.finfo(float).eps

sinc = np.sin(n * np.pi / 2) / (n * np.pi + eps)
sinc[N//2] = 1/2

b = sinc * win
print(b)

w, h = freqz(b, worN=1024, fs=44100)
h = 20 * np.log10(abs(h))

plot.figure(f'{taps} taps')
plot.plot(w, h)
plot.xlabel('Hz')
plot.ylabel('dB')
plot.show()
