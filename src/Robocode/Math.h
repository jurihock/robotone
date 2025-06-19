#pragma once

#include <Robocode/Arctangent.h>

inline double angle(const std::complex<double>& z)
{
  #if defined(ENABLE_ARCTANGENT_APPROXIMATION)
    return Arctangent::atan2(z);
  #else
    return std::arg(z);
  #endif
}

inline double wrap(const double angle)
{
  const double pi = 2 * std::numbers::pi;

  return angle - pi * std::floor(angle / pi + 0.5);
}

// (2 * pi) / sr
inline double hertz_to_radian(const double samplerate)
{
  return (2 * std::numbers::pi) / samplerate;
}

// sr / (2 * pi)
inline double radian_to_hertz(const double samplerate)
{
  return samplerate / (2 * std::numbers::pi);
}

// https://newt.phys.unsw.edu.au/jw/notes.html
inline double midi_to_hertz(const double midi, const double concertpitch)
{
  return std::pow(2, (midi - 69) / 12) * concertpitch;
}
