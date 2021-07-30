/*
function sendRequest(data) {
  const grpc = require('grpc');

  const protoPath = require('path').join(__dirname);
  const proto = grpc.load({root: protoPath, file: 'tc.proto' });

  //Create a new client instance that binds to the IP and port of the grpc server.
  const client = new proto.rpc.towncrier('128.84.84.208:12345', grpc.credentials.createInsecure());

  client.participate(data, (error, response) => {
    if (!error) {
      console.log("Succeed!");
    } else {
      console.log("Error:", error.message);
    }
  });
}
*/

function stringToArrayBuffer(string) {
  var buffer = new ArrayBuffer(string.length);
  var bufView = new Uint8Array(buffer);
  for (var i=0; i < string.length; i++) {
    bufView[i] = string.charCodeAt(i);
  }
  return buffer;
}

function arrayBufferToString(buffer) {
  return String.fromCharCode.apply(null, new Uint8Array(buffer));
}

const addr = 'ws://localhost:9001'

const tabStorage = {};
const networkFilters = {
  urls: [
    "https://www.coinbase.com/api/v2/user?",
    "https://onlinebanking.mtb.com/Accounts/*"
  ]
};

chrome.webRequest.onSendHeaders.addListener((details) => {
  console.log(details.url);
  if (typeof details.requestHeaders !== 'undefined') {
    for (var i = 0; i < details.requestHeaders.length; ++i) {
      console.log(details.requestHeaders[i].name, details.requestHeaders[i].value);
    }
  }
  var ws = new WebSocket(addr);
  ws.onopen = function(evt) { 
    console.log("Preparing "); 
    ws.send("Hello W!");
  };

  ws.onmessage = function(evt) {
    console.log( "Received Message: " + evt.data);
    ws.close();
  };

  ws.onclose = function(evt) {
    console.log("Connection closed.");
  };      

/*
  chrome.socket.create("tcp", (createInfo) => {
    var socketId = createInfo.socketId;
    chrome.socket.connect(socketId, hostname, port, (result) => {
      if (result === 0) {
        var requestString = "GET / HTTP/1.1\r\nHost: "+hostname+"\r\nConnection: close\r\n\r\n";
        var requestBuffer = stringToArrayBuffer(requestString);
        chrome.socket.write(socketId, requestBuffer, (writeInfo)) {});
      }
    });
  });
*/
//sendRequest(details.url);
}, networkFilters, ["requestHeaders", "extraHeaders"]);
