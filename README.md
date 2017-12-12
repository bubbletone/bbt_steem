teem
"Blockchain in Telecom" - Steem repositary


## Compile and install test node

Install and run test node on Ubunty 16.04. If you want to use other OS, please follow instructions from <a href="https://github.com/steemit/steem/blob/master/doc/building.md">https://github.com/steemit/steem/blob/master/doc/building.md</a>

### 1. Install steem (a part of instruction from **building.md**)
This part is taken from https://github.com/steemit/steem/blob/master/doc/building.md#building-on-ubuntu-1604.

####Run
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
    
####Run
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

#### Run (optional, for developers)
sudo apt-get install -y \
    doxygen \
    libncurses5-dev \
    libreadline-dev \
    perl

#### Run (make steemd) 

 cd $HOME
 git clone https://github.com/steemit/steem
cd steem
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc) steemd

### 2. Checkout and making the bbt_plugin 
 cd $HOME
 git clone https://github.com/bubbletone/bbt_steem


### 3. Test cycle
#### Info 
  # All packages and code for test node will reside in **\$HOME/steem** (main steem code) and **\$HOME/bbt_steem** (bbt plugin). Now, bbt_plugin(bbt_steem/libraries/) is simply copied over **steem/libraries/plugins/bbt_plugin** every time we update the source 

#### Run
cd \$HOME/bbt_steem
 # edit code...
 # edit code...
\$HOME/bbt_steem/steem_testnet/rebuild_steemd_with_plugin.sh

#### Run (test steemd daemon)
#### Info
Compiled *steemd* daemon uses **config.ini** from **bbt_steem/steem_testnet/config.ini**, and this config is copied to steemd workind dir every run, to make first simple test cycle. 

#### Run
cd \$HOME/steem/build
\$HOME/bbt_steem/steem_testnet/testrun_steemd.sh

#### Info
Now we can test working node, sending several request to it. Full requests pack for test is placed in **steem_testnet/tests/bbtone_test1.py**. 
*steemd* with config runs as websockets server, opened on 8090 port (look into steemd logs for exact info). So testing one singel API can be made with simple **curl** request. 

#### Run

 # for example
curl --data '{"jsonrpc": "2.0", "params": [4, "get_service_offers_of_given_operator_id", [1]], "id":2, "method":"call"}' http://127.0.0.1:8090

 # first, you need to get API ID, assigned by steemd to bbt_plugin_api (number of APIs family). 
 # If result is something like *{"id":1,"result":4}* - then API ID for bbtone_api is 4.
 # You should use this number in all bbtone_api calls in future.
 
curl --data '{"jsonrpc": "2.0", "params": [**1**, "get_api_by_name", ["bbtone_api"]], "id":1, "method":"call"}' http://127.0.0.1:8090








 



