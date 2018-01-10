#!/usr/bin/python3

import json
from pprint import pprint
import websocket
import socket
from websocket import create_connection

import base58
import binascii

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
    return {}

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
OFFER_PRICE = "%.3f TESTS" % random.random()

print("""\n[#] Operator '{}' (that will be an operator-assignee in future) broadcasts a transaction with service offer (with id: {}), offering service to other operators"""
     .format(OFFERING_OPERATOR, OFFER_ID_1))


res = get_api_response({'id': 2, 'params':  [API_NUM, 'create_service_offer',
                                            [OFFERING_OPERATOR, 
                                            OFFER_ID_1,
                                            '0xHEX_DIFFIEHELLMAN_HANDSHAKE',
                                            OFFER_TRANSACTION_TTL,
                                            OFFER_PRICE
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
o = random.choice(res)
REQUEST_OFFER_ID = o['id']


# [!!!!!!] PRICE, sent MUST BE exact string, as was placed in offer, f.ex. "0.1 TESTS" и "0.100 TESTS" are DIFFERENT
REQUEST_OFFER_PRICE = o['price']
print("\n[#] Requesting operator '{}' choose one 'offer' transaction(tx_id: {}, price: {})".format(REQUESTING_OPERATOR, REQUEST_OFFER_ID, REQUEST_OFFER_PRICE))


# check if there is enough available funds on requesting operator's balance
res = get_api_response({'id': 1, 'params': [0, 'get_accounts', [[REQUESTING_OPERATOR]]]})['result']
REQUESTING_OPERATOR_BALANCE = res[0]['balance']
print("[STAGE] The balance of requesting operator '{}': {}\n".format(REQUESTING_OPERATOR, REQUESTING_OPERATOR_BALANCE))



CUSTOMER_NAME = 'vasiliy'
CUSTOMER_PUBLIC_KEY_WIF = 'TST6LLegbAgLAy28EHrffBVuANFWcFgmqRMW13wBmTExqFE9SCkg4' # the one from "initminer"


wif_to_hex = lambda wif_key: binascii.b2a_hex(base58.b58decode(wif_key)).decode("ascii")
# for example: 'TST6LLegbAgLAy28EHrffBVuANFWcFgmqRMW13wBmTExqFE9SCkg4': '287e2ccc00a2040a7a97ae8544168065e7bd197fa43a238344f76fc6992ef1d780170525c81f6d'
CUSTOMER_PUBLIC_KEY_HEX = wif_to_hex(CUSTOMER_PUBLIC_KEY_WIF)

print("\n[#] Requesting operator '{}' attaches service request for customer '{}' (public_key: {}), using received offer (id: {}) and trying to send (credits: {}) with it"
      .format(REQUESTING_OPERATOR, CUSTOMER_NAME, CUSTOMER_PUBLIC_KEY_HEX, REQUEST_OFFER_ID, REQUEST_OFFER_PRICE))

res = get_api_response({'id': 1, 'params': [API_NUM, 'attach_request_to_service_offer', [REQUESTING_OPERATOR, REQUEST_OFFER_ID, REQUEST_TRANSACTION_TTL, REQUEST_OFFER_PRICE, CUSTOMER_NAME, CUSTOMER_PUBLIC_KEY_HEX]]})['result']
REQUEST_TX = res


# check balance after attaching request
res = get_api_response({'id': 1, 'params': [0, 'get_accounts', [[REQUESTING_OPERATOR]]]})['result']
REQUESTING_OPERATOR_BALANCE = res[0]['balance']
print("[STAGE] Now, the balance of requesting operator '{}': {}\n".format(REQUESTING_OPERATOR, REQUESTING_OPERATOR_BALANCE))


print("\n[#] Offering operator '{}' permanently monitors blockchain, looking for active service requests, attached to its offers...".format(OFFERING_OPERATOR))
REQUESTS_LIMIT = 42
res = get_api_response({'id': 1, 'params': [API_NUM, 'get_active_service_requests_attached_to_offers_of_given_operator_name', [OFFERING_OPERATOR, REQUESTS_LIMIT]]})['result']
print("\n[#] Offering operator '{}' found {} active requests: [{}]".format(OFFERING_OPERATOR, len(res), " ". join(str('(id: ' + str(x['id']) + ', user_id: "' + x['user_id'] + '", max_credits: ' + x['max_credits']) for x in res)))

SERVICE_REQUEST_TX = random.choice(res)


print("\n[#] Offering operator '{}' chooses one service request(tx_id: {}, max_credits: {}), attached to one of his offers".format(OFFERING_OPERATOR, SERVICE_REQUEST_TX['id'], SERVICE_REQUEST_TX['max_credits']))
pprint(SERVICE_REQUEST_TX)
exit(0);


res = get_api_response({'id': 1, 'params': [API_NUM, 'attach_charge_to_service', [SERVICE_REQUEST_TX['tx_id']]]})['result']

CHARGE_TX = res

print("\n[#] One of operators (assignee or issuer) attach 'refund' TX to any previous TX")
res = get_api_response({'id': 1, 'params': [API_NUM, 'refund_and_close_request', [CHARGE_TX['tx_id']]]})['result']

# pprint(res)
exit(0)

