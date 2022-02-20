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

#wallet_addr = bytes.fromhex('0000000000000000000000000000000000000000')


#source_dict = {
#    "https://secure.ssa.gov/myssa/myprofi": 12,
#    "https://accounts.coinbase.com/api/v1": 13,
#    "https://onlinebanking.mtb.com/Accoun": 14,
#    "https://secure01b.chase.com/svc/rr/p": 15,
#    "https://api.spotify.com/v1/playlists": 16,
#    "https://otc.tax.ny.gov/webapp/wcs/st": 17,
#}

def encrypt(msg):
    output = subprocess.check_output([command, sgx_pk, msg])
    print("Encrypted header: ", output)
    return output

def rpc_call(data):
    channel = grpc.insecure_channel(sgx_server, options=(('grpc.enable_http_proxy', 0),))
    stub = tc_pb2_grpc.towncrierStub(channel)
    message = tc_pb2.Data(data = data)
    res = stub.id_nft(message)
    return res.data


async def request(websocket, path):
    data = await websocket.recv()
    print("Received data: ", data)
    resp = rpc_call(data)
    await websocket.send(resp)
    print("Sent response: ", resp)

class handler(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
    
    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        content = self.rfile.read(content_length) # <--- Gets the data itself
        data = content.decode()[1:-1]
        print("Received data:", data)
        resp = rpc_call(data)
        #resp = b"OK"
        self._set_response()
        self.wfile.write(resp.encode())


# Use Caddy to host HTTPS: https://caddyserver.com/docs/install, https://caddyserver.com/docs/quick-starts/https#:~:text=The%20reverse%2Dproxy%20command
# run $ sudo caddy reverse-proxy --from sgx.candid.id --to localhost:8080
if __name__ == "__main__":
    args = sys.argv[1:]
    if len(args) == 1 and args[0] == "-ws":
        start_server = websockets.serve(request, ws_host, ws_port)
        asyncio.get_event_loop().run_until_complete(start_server)
        asyncio.get_event_loop().run_forever()
    else:
        with HTTPServer(('', 8080), handler) as server:
            try:
                server.serve_forever()
            except KeyboardInterrupt:
                pass
            server.server_close()
    
