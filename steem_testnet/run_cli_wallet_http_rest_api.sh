#!/bin/sh
TEST_STEEM_CLI_WALLET_REST_API_IP_PORT=127.0.0.1:6789
/usr/local/bin/cli_wallet \
	--rpc-http-endpoint="$TEST_STEEM_CLI_WALLET_REST_API_IP_PORT" \
	--rpc-http-allowip 127.0.0.1

