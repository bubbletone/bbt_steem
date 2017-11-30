#!/bin/bash
cp $HOME/bbt_steem/steem_testnet/config.ini /var/lib/steemd/config.ini && programs/steemd/steemd -d /var/lib/steemd --enable-plugin bbtone $1
