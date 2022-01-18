import asyncio
import socketserver
import websockets
import grpc
import sys

import tc_pb2
import tc_pb2_grpc

import os
import base64
from ecies.utils import generate_eth_key, generate_key
from ecies import encrypt, decrypt
from coincurve import PrivateKey
from Crypto.Cipher import AES
import web3

import subprocess
command = './hybrid-enc/henc'
sgx_pk = 'BLtIrjcmxXNzRKVLNGP+xJnLEIp9EboTe6PH0EO9bX4UmU9gRio/kVUHSbsq5UEfIrf5vueZVqRjwwitUI81V98=' 
sgx_server = 'localhost:12345'
proxy_port = 9001
proxy_host = "0.0.0.0"

wallet_addr = bytes.fromhex('0000000000000000000000000000000000000000')


source_dict = {
    "https://secure.ssa.gov/OSSS/xml/down": 12,
    "https://accounts.coinbase.com/api/v1": 13,
    "https://onlinebanking.mtb.com/Accoun": 14,
    "https://secure01b.chase.com/svc/rr/p": 15,
    "https://api.spotify.com/v1/playlists": 16,
    "https://otc.tax.ny.gov/webapp/wcs/st": 17,
}

def encrypt(msg):
    output = subprocess.check_output([command, sgx_pk, msg])
    print("Encrypted header: ", output[8:-1])
    return output[8:-1]

def rpc_call(data):
    channel = grpc.insecure_channel(sgx_server, options=(('grpc.enable_http_proxy', 0),))
    stub = tc_pb2_grpc.towncrierStub(channel)
    message = tc_pb2.Data(source = 0, addr = wallet_addr, data = data)
    res = stub.participate(message)


async def request(websocket, path):
    data = await websocket.recv()
    print("Receive data: ", data)
    rpc_call(data)
    print("Encrypted data: ", data)


if __name__ == "__main__":
    args = sys.argv[1:]
    
    if len(args) >= 1 and web3.Web3.isAddress(args[0]):
        wallet_addr = bytes.fromhex(args[0][2:])

    start_server = websockets.serve(request, "0.0.0.0", proxy_port) # 0.0.0.0 for remote connection
    # start_server = websockets.serve(request, "localhost", proxy_port)
    asyncio.get_event_loop().run_until_complete(start_server)
    asyncio.get_event_loop().run_forever()
