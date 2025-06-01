#pragma once

#if defined(ENABLE_JINGLE_CHRONOMETER)

  #include <juce_jingles/utils/Timer.h>

  BEGIN_JUCE_JINGLES_NAMESPACE

  static Timer<std::chrono::microseconds> juce_jingles_chronometer;

  static inline std::string juce_jingles_chronometry(const double samplerate, const int blocksize)
  {
    const double period = (samplerate > 0) ? (blocksize / samplerate) : 0;
    const std::string stats = juce_jingles_chronometer.str();

    juce_jingles_chronometer.cls();

    std::ostringstream result;

    result.setf(result.flags() | std::ios::fixed);
    result.precision(3);

    result << stats << " | ";
    result << (period * 1e+6)  << " us ";
    result << "bs=" << blocksize  << " ";
    result << "sr=" << samplerate;

    return result.str();
  }

  END_JUCE_JINGLES_NAMESPACE

  #define TIC(...) do { JUCE_JINGLES_NAMESPACE::juce_jingles_chronometer.tic(); } while (false)
  #define TOC(...) do { JUCE_JINGLES_NAMESPACE::juce_jingles_chronometer.toc(); } while (false)
  #define LAP(...)    ( JUCE_JINGLES_NAMESPACE::juce_jingles_chronometer.lap() > 10e+6 )

  #define CHRONOMETRY(...) JUCE_JINGLES_NAMESPACE::juce_jingles_chronometry(__VA_ARGS__)

#else

  #define TIC(...)
  #define TOC(...)
  #define LAP(...) false

  #define CHRONOMETRY(...) "=^..^="

#endif
