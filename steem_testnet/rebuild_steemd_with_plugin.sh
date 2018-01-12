#!/bin/bash

mkdir -p $HOME/steem/build
cd $HOME/steem/build

# heh, sorry
cp -r $HOME/bbt_steem/steem/libraries/plugins/bbtone $HOME/steem/libraries/plugins/
cp --preserve=timestamp $HOME/bbt_steem/steem/libraries/protocol/include/steemit/protocol/config.hpp $HOME/steem/libraries/protocol/include/steemit/protocol/config.hpp

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_STEEM_TESTNET=ON ..

make -j$(nproc) steemd
