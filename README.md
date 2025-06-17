# Robotone

![language](https://img.shields.io/badge/plugin-AU%20LV2%20VST3-blue)
![license](https://img.shields.io/github/license/jurihock/robotone?color=blue)
![build](https://img.shields.io/github/actions/workflow/status/jurihock/robotone/build.yml?branch=main&label=build)
![tag](https://img.shields.io/github/v/tag/jurihock/robotone?color=gold)

The experimental *Robotone* audio effect plugin applies a fixed pitch onto a sound controlled by a [MIDI](https://newt.phys.unsw.edu.au/jw/notes.html) note event. Originally this technique puts zero phase values on every [STFT](https://en.wikipedia.org/wiki/Short-time_Fourier_transform) frame before reconstruction [[1](#1),[2](#2),[3](#3)]. However, the phase zeroing effect is emulated in *Robotone* using the [Sliding DFT](https://www.dsprelated.com/showarticle/1396.php) as implemented in [SDFT](https://github.com/jurihock/sdft). This enables polyphonic usage in a single processing step.

The basic idea behind this effect is to reinterpret the frequency axis by replacing the DFT bin frequencies with overtone series as played on a MIDI keyboard. When applied to the vocal input, this particular modification produces a robotic sound at a fixed pitch. Additional phase vocoder based corrections result in pitch shifting. Unfortunately, the pitch shifted output does not preserve the formants. This issue might be resolved in a future version.

However both effect variants can be used to add a harmonic touch to vocals in a musical way. I suppose so at least...

## References

1. <span id="1">Vincent Verfaille, Daniel Arfib (2001). Adaptive Digital Audio Effects. https://dafx.de/paper-archive/2001/papers/verfaille_a.pdf</span>

2. <span id="2">Vincent Verfaille, Udo Zölzer, Daniel Arfib (2006). Adaptive Digital Audio Effects. https://hal.science/hal-00095922/document</span>

3. <span id="3">Udo Zölzer (2011). Digital Audio Effects. https://www.dafx.de/DAFX_Book_Page_2nd_edition/index.html</span>

## License

*Robotone* is licensed under the terms of the GPL license.
For details please refer to the accompanying [LICENSE](LICENSE) file distributed with *Robotone*.
