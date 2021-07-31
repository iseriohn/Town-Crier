import asyncio
import websockets
import grpc

import tc_pb2
import tc_pb2_grpc

import os
import base64
from ecies.utils import generate_eth_key, generate_key
from ecies import encrypt, decrypt
from coincurve import PrivateKey
from Crypto.Cipher import AES

import subprocess
command = './hybrid-enc/henc'
sgx_pk = 'BLtIrjcmxXNzRKVLNGP+xJnLEIp9EboTe6PH0EO9bX4UmU9gRio/kVUHSbsq5UEfIrf5vueZVqRjwwitUI81V98=' 

sgx_server = '128.84.84.208:12345'
proxy_port = 9001

sgx_pubkey = base64.b64decode('BLtIrjcmxXNzRKVLNGP+xJnLEIp9EboTe6PH0EO9bX4UmU9gRio/kVUHSbsq5UEfIrf5vueZVqRjwwitUI81V98=')

def encrypt(msg):
    output = subprocess.check_output([command, sgx_pk, msg])
    print(output)
    print(output[8:-1])
    return output[8:-1]

def rpc_call(data):
    channel = grpc.insecure_channel(sgx_server, options=(('grpc.enable_http_proxy', 0),))
    stub = tc_pb2_grpc.towncrierStub(channel)
    message = tc_pb2.Data(data=data)
    res = stub.participate(message)


async def hello(websocket, path):
    data = await websocket.recv()
    print(f"{data}\n{len(data)}")
    rpc_call(encrypt(data))

#rpc_call(encrypt("hi\ hi"))    

start_server = websockets.serve(hello, "localhost", proxy_port)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
