#pragma once

// enable oSDFT by default #1
// #include <Robotone/DSP/SDFT/msdft.h>
#include <Robotone/DSP/SDFT/osdft.h>

template <typename T, typename F>
using SDFT = sdft::SDFT<T, F>;
