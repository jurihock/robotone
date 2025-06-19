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
template<typename T>
inline double midi_to_hertz(const T midi, const double concertpitch)
{
  static const double A4 = 69;

  return std::pow(2, (midi - A4) / 12) * concertpitch;
}

template<typename T>
inline double note_to_hertz(const std::string& note, const T octave, const double concertpitch)
{
  static const double C0 = std::pow(2, double(-1) * (9 + 4 * 12) / 12);

  static const std::map<std::string, int> notes =
  {
    { "C",   0 },
    { "C#",  1 },
    { "D",   2 },
    { "D#",  3 },
    { "E",   4 },
    { "F",   5 },
    { "F#",  6 },
    { "G",   7 },
    { "G#",  8 },
    { "A",   9 },
    { "A#", 10 },
    { "B",  11 }
  };

  const double semitone = notes.at(note);

  return std::pow(2, (semitone / 12) + octave) * C0 * concertpitch;
}
