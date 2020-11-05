#!/bin/sh

#lucaceriani

echo "Downloadgin Boost 1.74.0..."
wget "https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz"

echo "Extracting Boost..."
tar -xf boost_1_74_0.tar.gz

echo "Installing Boost..."
cd boost_1_74_0
./bootstrap.sh --prefix=$(pwd)/../boost
./b2 install

