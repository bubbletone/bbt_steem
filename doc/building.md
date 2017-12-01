# Installation on clean Ubuntu 16.04
# Steem install taken from https://github.com/steemit/steem/blob/master/doc/building.md

## Steem install 
### Install packages
sudo apt-get install -y \
    autoconf \
    automake \
    cmake \
    g++ \
    git \
    libssl-dev \
    libtool \
    make \
    pkg-config \
    python3 \
    python3-jinja2

### Boost packages (also required)
sudo apt-get install -y \
    libboost-chrono-dev \
    libboost-context-dev \
    libboost-coroutine-dev \
    libboost-date-time-dev \
    libboost-filesystem-dev \
    libboost-iostreams-dev \
    libboost-locale-dev \
    libboost-program-options-dev \
    libboost-serialization-dev \
    libboost-signals-dev \
    libboost-system-dev \
    libboost-test-dev \
    libboost-thread-dev

### Optional packages (not required, but will make a nicer experience)
sudo apt-get install -y \
    doxygen \
    libncurses5-dev \
    libreadline-dev \
    perl

### Main steem repositary checkout
git clone https://github.com/steemit/steem
cd steem
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc) steemd

####### wait....


# Adding bbtone_plugin
git clone https://github.com/bubbletone/bbt_steem.git

######### copies bbt_plugin dir into steem dir and runs make of steem with our plugin
bbt_steem/steem_testnet/rebuild_steemd_with_plugin.sh






