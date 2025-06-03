#pragma once

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

BEGIN_JUCE_JINGLES_NAMESPACE

template<class> struct SupportedTimerUnit : std::false_type
{
  static constexpr const char unit[] = "n.a.";
};

template<> struct SupportedTimerUnit<std::chrono::seconds> : std::true_type
{
  static constexpr const char unit[] = "s";
};

template<> struct SupportedTimerUnit<std::chrono::milliseconds> : std::true_type
{
  static constexpr const char unit[] = "ms";
};

template<> struct SupportedTimerUnit<std::chrono::microseconds> : std::true_type
{
  static constexpr const char unit[] = "us";
};

template<> struct SupportedTimerUnit<std::chrono::nanoseconds> : std::true_type
{
  static constexpr const char unit[] = "ns";
};

template<class T>
class Timer final
{

  static_assert(SupportedTimerUnit<T>::value, "s,ms,us,ns");

public:

  Timer(const size_t capacity = 1024 * 1024 / sizeof(double))
  {
    data.reserve(capacity);
  }

  Timer(const Timer& other)
  {
    time = other.time;

    data.reserve(other.data.capacity());
    data.assign(other.data.begin(), other.data.end());
  }

  size_t capacity() const
  {
    return data.capacity();
  }

  size_t size() const
  {
    return data.size();
  }

  void cls()
  {
    data.clear();
  }

  double lap() const
  {
    if (data.empty())
    {
      return 0;
    }

    const std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - time.front();
    const double value = std::chrono::duration_cast<T>(duration * 1e+3).count() * 1e-3;

    return value;
  }

  void tic()
  {
    time.back() = std::chrono::steady_clock::now();

    if (data.empty())
    {
      time.front() = time.back();
    }
  }

  void toc()
  {
    const std::chrono::steady_clock::duration duration = std::chrono::steady_clock::now() - time.back();
    const double value = std::chrono::duration_cast<T>(duration * 1e+3).count() * 1e-3;

    data.push_back(value);
  }

  std::string str() const
  {
    static const std::string unit = SupportedTimerUnit<T>::unit;

    const double sum = std::accumulate(data.begin(), data.end(), 0.0);
    const double prod = std::inner_product(data.begin(), data.end(), data.begin(), 0.0);

    const double mean = sum / data.size();
    const double stdev = std::sqrt(prod / data.size() - mean * mean);

    std::ostringstream result;
    result.setf(result.flags() | std::ios::fixed);
    result.precision(3);
    result << mean << " \u00b1 " << stdev << " " << unit << " n=" << data.size();

    return result.str();
  }

private:

  std::array<std::chrono::time_point<std::chrono::steady_clock>, 2> time;
  std::vector<double> data;

};

END_JUCE_JINGLES_NAMESPACE
