#pragma once

#include <Robocode/Header.h>

#include <samplerate.h>

class SRC final
{

public:

  SRC(const double samplerate_from, const double samplerate_to, const size_t blocksize);

  bool applicable() const;
  double samplerate() const;

  void resample(const std::span<const float> input, const std::span<float> output,
                const std::function<void(const std::span<const float> input, const std::span<float> output)>& callback);

  void reset();

private:

  const std::pair<double, double> samplerates;

  struct src_t
  {
    std::unique_ptr<SRC_STATE, decltype(&src_delete)> state {nullptr, src_delete};
    SRC_DATA data;
    std::vector<float> buffer;
  };

  src_t downsampling;
  src_t upsampling;

};
