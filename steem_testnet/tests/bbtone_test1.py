#!/usr/bin/python3

import json
from pprint import pprint
import websocket
import socket
from websocket import create_connection


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


print("\n[#] Operator (that will be an operator-assignee in future) broadcasts a transaction with service offer, offering service to other operators")
OPERATOR_ASSIGNEE_ID = "666666"
res = get_api_response({'id': 2, 'params': [API_NUM, 'broadcast_service_offer', [OPERATOR_ASSIGNEE_ID]]})['result']
print("[STAGE] broadcasted offer from operator_id: {}, tx_id: {}\n".format(OPERATOR_ASSIGNEE_ID, res['tx_id']))


print("\n[#] Other operator(future operator-issuer) gets list of offers of given operator. He knows which one he needs, beacuse he has a list of operators, suitable for providing service to client")
res = get_api_response({'id': 1, 'params': [API_NUM, 'get_service_offers_of_given_operator_id', [OPERATOR_ASSIGNEE_ID]]})['result']

SERVICE_OFFER_TX = res[0]

print("\n[#] Operator-isser choose one 'offer' transaction(tx_id: {}, price: {}) and attach its service request to it".format(SERVICE_OFFER_TX['tx_id'], SERVICE_OFFER_TX['price']))
res = get_api_response({'id': 1, 'params': [API_NUM, 'attach_service_request_to_service_offer', [SERVICE_OFFER_TX['tx_id']]]})['result']


print("\n[#] Operator-assignee is monitoring blockchain, looking for active service request for him(search transactions, attached to his offers)")
res = get_api_response({'id': 1, 'params': [API_NUM, 'get_active_service_requests_of_given_operator_id', [OPERATOR_ASSIGNEE_ID]]})['result']

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



