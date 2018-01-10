#!/bin/bash

mkdir -p $HOME/steem/build
cd $HOME/steem/build

# heh, sorry
cp -R $HOME/bbt_steem/steem/libraries/plugins/bbtone $HOME/steem/libraries/plugins/

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_STEEM_TESTNET=ON ..

make -j$(nproc) steemd
