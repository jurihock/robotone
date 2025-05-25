#!/bin/bash

sudo apt update

# build essentials
sudo apt install --yes \
  build-essential \
  cmake \
  pkg-config

# https://github.com/juce-framework/JUCE/blob/master/docs/Linux%20Dependencies.md
sudo apt install --yes \
  ladspa-sdk \
  libasound2-dev \
  libcurl4-openssl-dev  \
  libfontconfig1-dev \
  libfreetype-dev \
  libglu1-mesa-dev \
  libjack-jackd2-dev \
  libwebkit2gtk-4.1-dev \
  libx11-dev \
  libxcomposite-dev \
  libxcursor-dev \
  libxext-dev \
  libxinerama-dev \
  libxrandr-dev \
  libxrender-dev \
  mesa-common-dev
