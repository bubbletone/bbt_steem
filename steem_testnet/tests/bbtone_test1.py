#!/usr/bin/python3

import json
from pprint import pprint
import websocket
import socket
from websocket import create_connection

import random
import time

def get_new_max_random_offer_id():
    # TODO - in real implementaion - be careful with id generation

    # don't do this in prod often
    random.seed(time.time())
    return int(random.random() * 0xFFFFFFFFFFFFFFFF)

    

BBT_NODE_WS_ENDPOINT = 'ws://10.197.197.59:8090'
# websocket.enableTrace(True)
# API_NUM = 1


def get_api_response(data):
    ws = create_connection(BBT_NODE_WS_ENDPOINT)
    # simple open-close-connection, DON'T USE IT in production, only for tests
    try:
        data['jsonrpc'] = '2.0'
        data['method'] = 'call'
        result_field = 'result'
        json_str = json.dumps(data)
        print("[DEBUG] WS_SEND: {}".format(json_str))
        ws.send(json_str)
        response_str = ws.recv()
        ws.close()
        print("[DEBUG] WS_RECV: {}".format(response_str))
        res = json.loads(response_str)
        return res
    except Exception as e:
        print("Error getting ws response from {}: {}".format(BBT_NODE_WS_ENDPOINT, e))
    finally:
        ws.close()
    return None 

# we need to get ID of custom bbtone_api and use it in request to API functions later
print("\n[#] Any participant receive API id on node, checking if it's active")
API_NUM = get_api_response({'id': 1, 'params': [1, 'get_api_by_name', ['bbtone_api']]})['result']
if API_NUM is None:
    print("No API id returned, is bbtone_api is enabled on bbt node?")
    exit(1)

## YOU CAN FIND ALL API FUNCTIONS in "bbtone_plugin.hpp" file



OFFER_TRANSACTION_TTL = 100 # 100 sec until offer is "alive"

OFFERING_OPERATOR = "initminer"
OFFER_ID_1=get_new_max_random_offer_id()

print("""\n[#] Operator '{}' (that will be an operator-assignee in future) broadcasts a transaction with service offer (with id: {}), offering service to other operators"""
     .format(OFFERING_OPERATOR, OFFER_ID_1))


res = get_api_response({'id': 2, 'params':  [API_NUM, 'create_service_offer',
                                            [OFFERING_OPERATOR, 
                                            OFFER_ID_1,
                                            '0xHEX_DIFFIEHELLMAN_HANDSHAKE',
                                            OFFER_TRANSACTION_TTL,
                                            "0.1 TESTS"
                                            ]]})['result']

print("[STAGE] broadcasted offer from operator_id: {}, tx_id: {}\n".format(OFFERING_OPERATOR, res['tx_id']))
# in log must be something like:
# [STAGE] broadcasted offer from operator_id: initminer, tx_id: 93cbdf05beee88bec18677a9dc27a5d6175d92a8





SEARCH_LIMIT = 42
print("""\n[#] Operator '{}' (that will be an requesting operator in future) searches N < {} suitable offers in blockchain"""
     .format(OFFERING_OPERATOR, SEARCH_LIMIT))

res = get_api_response({'id': 1, 'params': [API_NUM, 'get_service_offers_by_operator_name', [OFFERING_OPERATOR, SEARCH_LIMIT]]})['result']

print("[STAGE] found {} offers from operator '{}': [{}]\n".format(len(res), OFFERING_OPERATOR, " ". join(str('(' + str(x['id']) + ', "' + x['price'] + '")') for x in res)))

# Get random offer from found offers, get its id, params and attach request to it 
REQUESTING_OPERATOR = "initminer"

REQUEST_TRANSACTION_TTL = 600
o =random.choice(res)
OFFER_ID = o['id']
OFFER_PRICE = o['price']

print("\n[#] Requesting operator '{}' choose one 'offer' transaction(tx_id: {}, price: {})".format(REQUESTING_OPERATOR, OFFER_ID, OFFER_PRICE))

CUSTOMER_NAME = 'Vasiliy'
CUSTOMER_PUBLIC_KEY_WIF = 'TST6LLegbAgLAy28EHrffBVuANFWcFgmqRMW13wBmTExqFE9SCkg4' # the one from "initminer"

import base58
import binascii
# using this snippet to convert base58(WIF) key format in config.ini to hex:
CUSTOMER_PUBLIC_KEY_HEX = binascii.b2a_hex(base58.b58decode(CUSTOMER_PUBLIC_KEY_WIF)).decode("ascii")
# base58.b58decode(b'TST6LLegbAgLAy28EHrffBVuANFWcFgmqRMW13wBmTExqFE9SCkg4').encode("hex")
# '287e2ccc00a2040a7a97ae8544168065e7bd197fa43a238344f76fc6992ef1d780170525c81f6d'

print("\n[#] Requesting operator '{}' attaches service request for customer '{}' (public_key: {}), using received offer id {} and trying to send {} credits with it"
      .format(REQUESTING_OPERATOR, CUSTOMER_NAME, CUSTOMER_PUBLIC_KEY_HEX, OFFER_ID, OFFER_PRICE))

res = get_api_response({'id': 1, 'params': [API_NUM, 'start_request', [REQUESTING_OPERATOR, int(OFFER_ID), REQUEST_TRANSACTION_TTL, OFFER_PRICE, CUSTOMER_NAME, CUSTOMER_PUBLIC_KEY_HEX]]})['result']
pprint(res)

exit(0);

print("\n[#] Operator-assignee is monitoring blockchain, looking for active service request for him(search transactions, attached to his offers)")
res = get_api_response({'id': 1, 'params': [API_NUM, 'get_active_service_requests_of_given_operator_id', [OFFERING_OPERATOR]]})['result']

SERVICE_REQUEST_TX = res[0]

print("\n[#] Operator-assignee choose one service request(tx_id: {}, reserve: {}), attached to one of his offers and attach 'charge' transaction to it".format(SERVICE_REQUEST_TX['tx_id'], SERVICE_REQUEST_TX['reserve']))
res = get_api_response({'id': 1, 'params': [API_NUM, 'attach_service_request_to_service_offer', [SERVICE_REQUEST_TX['tx_id']]]})['result']

CHARGE_TX = res

print("\n[#] One of operators (assignee or issuer) attach 'refund' TX to any previous TX")
res = get_api_response({'id': 1, 'params': [API_NUM, 'refund_and_close_request', [CHARGE_TX['tx_id']]]})['result']

# pprint(res)
exit(0)


#

# print("\n\n" + offer_tx_id + "\n\n")

# curl --data '{"jsonrpc": "2.0", "params": [4, "get_active_service_requests_of_given_account", [1]], "id":2, "method":"call"}' http://127.0.0.1:8090

# we have two accounts - "operator_assignee" and "operator_requestor"

# "operator_assignee" publish offers in blockchain
# "operator_requestor" search for offers in blockchain, chooses one, and attach its request to it
# "operator_assignee" search for his offers with attached requests on his in blockchain, chooses one, and attach his "charge" transaction to it
# "operator_requestor" waits for charge and attach "refund" transaction to "charge"

# get id of bbtone_api (usually 4)



