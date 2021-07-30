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

const tabStorage = {};
const networkFilters = {
  urls: [
    "https://www.coinbase.com/api/v2/user?"
  ]
};

chrome.webRequest.onSendHeaders.addListener((details) => {
  //console.log(details);
  if (typeof details.requestHeaders !== 'undefined') {
    for (var i = 0; i < details.requestHeaders.length; ++i) {
      console.log(details.requestHeaders[i].name, details.requestHeaders[i].value);
    }
  }
  //sendRequest(details.url);
}, networkFilters, ["requestHeaders"]);
