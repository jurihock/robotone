#pragma once

#include <Robocode/Header.h>

class Channel final
{

public:

  Channel();
  Channel(const size_t index, const double samplerate, const std::vector<double>& frequencies, const double concertpitch);
  Channel(const Channel& other);
  Channel& operator=(const Channel& other);

  double gain() const;

  void gain(const double gain);

  double synthesize(const std::span<std::complex<double>> dft,
                    const std::span<const double> dftfreqs,
                    const std::span<const double> pvcfreqs,
                    const int gestalt);

private:

  struct
  {
    size_t dftsize;

    double freq;
    double gain;

    double samplerate;
    double concertpitch;

    std::vector<double> chnfreqs;
    std::vector<double> phase[2];
  }
  config;

};
