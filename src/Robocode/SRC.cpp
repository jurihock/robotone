#include <Robocode/SRC.h>

SRC::SRC(const double samplerate_from, const double samplerate_to, const size_t blocksize) :
  samplerates({samplerate_from, samplerate_to})
{
  const auto setup = [](src_t& src, double ratio, size_t size, int quality = SRC_SINC_FASTEST)
  {
    int error = 0;

    src.state = std::unique_ptr<SRC_STATE, decltype(&src_delete)>(
      src_new(quality, 1, &error), src_delete);

    if (error)
    {
      throw std::runtime_error(
        src_strerror(error));
    }

    src.data.end_of_input = 0;
    src.data.src_ratio = ratio;
    src.buffer.resize(size);
  };

  if (applicable())
  {
    setup(downsampling, samplerates.second / samplerates.first, blocksize);
    setup(upsampling, samplerates.first / samplerates.second, blocksize);
  }
}

bool SRC::applicable() const
{
  return samplerates.first > samplerates.second;
}

double SRC::samplerate() const
{
  return applicable() ? samplerates.second : samplerates.first;
}

void SRC::resample(const std::span<const float> input, const std::span<float> output,
                   const std::function<void(const std::span<const float> input, const std::span<float> output)>& callback)
{
  if (!applicable())
  {
    callback(input, output);
    return;
  }

  int error;

  auto& src = downsampling;
  auto& dst = upsampling;

  src.data.data_in = input.data();
  src.data.data_out = src.buffer.data();

  src.data.input_frames = static_cast<long>(input.size());
  src.data.output_frames = static_cast<long>(src.buffer.size());

  error = src_process(src.state.get(), &src.data);

  if (error)
  {
    throw std::runtime_error(
      src_strerror(error));
  }

  assert_true(src.data.input_frames_used == src.data.input_frames, "Not all src frames are used!");

  auto n = static_cast<size_t>(std::max(src.data.output_frames_gen, 0L));

  callback({src.buffer.data(), n}, {dst.buffer.data(), n});

  dst.data.data_in = dst.buffer.data();
  dst.data.data_out = output.data();

  dst.data.input_frames = static_cast<long>(n);
  dst.data.output_frames = static_cast<long>(output.size());

  error = src_process(dst.state.get(), &dst.data);

  if (error)
  {
    throw std::runtime_error(
      src_strerror(error));
  }

  assert_true(dst.data.input_frames_used == dst.data.input_frames, "Not all dst frames are used!");

  auto m = static_cast<size_t>(std::max(dst.data.output_frames_gen, 0L));

  std::fill(output.begin() + m, output.end(), float(0));
}

void SRC::reset()
{
  if (downsampling.state)
  {
    int error = src_reset(downsampling.state.get());

    if (error)
    {
      throw std::runtime_error(
        src_strerror(error));
    }
  }

  if (upsampling.state)
  {
    int error = src_reset(upsampling.state.get());

    if (error)
    {
      throw std::runtime_error(
        src_strerror(error));
    }
  }
}
