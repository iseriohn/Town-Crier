import asyncio
import socketserver
import websockets
import grpc
import sys

import tc_pb2
import tc_pb2_grpc

import os
import base64

from http.server import BaseHTTPRequestHandler, HTTPServer

#import subprocess
#command = './hybrid-enc/henc'
#sgx_pk = 'BBarzLnfkPo3nLmRjT82ifMm8sbQpQSqavgD9omSAkorhxG+/8C7OqVKduXw2SZmBKYQYTNyqt6DwU4XSy6hkTw='
#sgx_server = 'localhost:12345'
sgx_server = 'localhost:8123'
ws_port = 9001
ws_host = "0.0.0.0" # 0.0.0.0 for remote connection

#!/usr/bin/env python3

from web3 import Web3
w3 = Web3(Web3.HTTPProvider("https://eth-rinkeby.gateway.pokt.network/v1/lb/621192c3146632003b6263af"));

abi = """[{"inputs":[{"internalType":"uint64","name":"subscriptionId","type":"uint64"}],"stateMutability":"nonpayable","type":"constructor"},{"inputs":[{"internalType":"address","name":"have","type":"address"},{"internalType":"address","name":"want","type":"address"}],"name":"OnlyCoordinatorCanFulfill","type":"error"},{"inputs":[],"name":"getRandomWordYo","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[{"internalType":"uint256","name":"requestId","type":"uint256"},{"internalType":"uint256[]","name":"randomWords","type":"uint256[]"}],"name":"rawFulfillRandomWords","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[],"name":"requestRandomWords","outputs":[],"stateMutability":"nonpayable","type":"function"},{"inputs":[{"internalType":"uint256","name":"","type":"uint256"}],"name":"s_randomWords","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"},{"inputs":[],"name":"s_requestId","outputs":[{"internalType":"uint256","name":"","type":"uint256"}],"stateMutability":"view","type":"function"}]"""




def encrypt(msg):
    output = subprocess.check_output([command, sgx_pk, msg])
    print("Encrypted header: ", output)
    return output

def rpc_call(data):
    channel = grpc.insecure_channel(sgx_server, options=(('grpc.enable_http_proxy', 0),))
    stub = tc_pb2_grpc.towncrierStub(channel)
    message = tc_pb2.Data(data = data)
    res = stub.raffle(message)
    return res.data


if __name__ == "__main__":
    con = w3.eth.contract(address="0x9563851e73c798caD7914BaD7092174dC5e263D5",abi=abi)

    rand = con.functions.getRandomWordYo().call()
    print(rand)
    print(rpc_call(str(rand)).encode())
