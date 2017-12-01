#!/bin/bash

# catalog with blockchain, p2p tmp files. Must reside on SSD!
STEEMD_WORK_DIR=/var/tmp/steemd
mkdir -p $STEEMD_WORK_DIR

mkdir -p $HOME/steem/build
cd $HOME/steem/build

# exit if we haven't write access
if [ -w $STEEMD_WORK_DIR ]; then echo "Dir: $STEEMD_WORK_DIR is ok"; else echo ""No write acces to dir: $STEEMD_WORK_DIR"; exit 3;"; fi

# copy config.ini from repo
echo "Replacing  $HOME/bbt_steem/steem_testnet/config.ini -> $STEEMD_WORK_DIR/config.ini. Disable if needed"
cp $HOME/bbt_steem/steem_testnet/config.ini $STEEMD_WORK_DIR/config.ini && programs/steemd/steemd -d $STEEMD_WORK_DIR --enable-plugin bbtone $1
