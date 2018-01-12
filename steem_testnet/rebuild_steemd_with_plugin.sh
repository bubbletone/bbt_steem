#!/bin/bash

mkdir -p $HOME/steem/build
cd $HOME/steem/build

# heh, sorry
FILES_AND_DIRS_TO_WRITE_OVER_EXISTING_IN_STEEM=("libraries/plugins/bbtone" "libraries/protocol/include/steemit/protocol/config.hpp")

for FILE_OR_DIR in "${FILES_AND_DIRS_TO_WRITE_OVER_EXISTING_IN_STEEM[@]}"
do
	set -x
	cp -r $HOME/bbt_steem/steem/$FILE_OR_DIR  $HOME/steem/$FILE_OR_DIR
	set +x
done

cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_STEEM_TESTNET=ON ..

make -j$(nproc) steemd
