#pragma once

#include <Robotone/DSP/Header.h>

class Channel final
{

public:

  Channel()
  {}

  Channel(const size_t index, const size_t dftsize, const double samplerate, const double concertpitch)
  {
    // https://newt.phys.unsw.edu.au/jw/notes.html
    const double hz = std::pow(2, (double(index) - 69) / 12) * concertpitch;

    // TODO
    // const double omega = 2 * std::numbers::pi * hz / samplerate;

    config.freq = hz;
    config.gain = 0;

    config.samplerate = samplerate;
    config.concertpitch = concertpitch;

    config.pvcfreqs.resize(dftsize);
    config.pvcphase.resize(dftsize);
  }

  Channel(const Channel& other) :
    config(other.config)
  {
  }

  Channel& operator=(const Channel& other)
  {
    if (this != &other)
    {
      config = other.config;
    }

    return *this;
  }

  double gain() const
  {
    return config.gain;
  }

  void gain(const double gain)
  {
    config.gain = gain;
  }

  double synthesize(const std::span<std::complex<double>> dft,
                    const std::span<const double> bins,
                    const std::span<const double> freqs)
  {
    const double tophase = (2 * std::numbers::pi) / config.samplerate;

    const double gain = config.gain;
    const double freq = config.freq;

    const auto& dftfreqs = bins;
    auto& pvcfreqs = config.pvcfreqs;
    auto& pvcphase = config.pvcphase;

    for (size_t i = 0; i < pvcfreqs.size(); ++i)
    {
      const double f0 = freqs[i];
      const double f1 = freq * i;

      pvcfreqs[i] = f1 * f0 / dftfreqs[i]; // f1 + (f0 - dftfreqs[i]) * (f1 / dftfreqs[i])
    }

    for (size_t i = 1; i < dft.size() - 1; ++i)
    {
      pvcphase[i] += pvcfreqs[i] * tophase; // omega[i] + (pvcfreqs[i] - dftfreqs[i]) * scale

      if (pvcfreqs[i] <= dftfreqs.front())
      {
        // dft[i] = 0;
        continue;
      }

      if (pvcfreqs[i] >= dftfreqs.back())
      {
        // dft[i] = 0;
        continue;
      }

      dft[i] += std::polar(gain, pvcphase[i]);
    }

    return gain;
  }

private:

  struct
  {
    double freq;
    double gain;

    double samplerate;
    double concertpitch;

    std::vector<double> pvcfreqs;
    std::vector<double> pvcphase;
  }
  config;

};
