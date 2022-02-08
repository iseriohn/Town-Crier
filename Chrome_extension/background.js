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

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

function str2ab(str) {
  const buf = new ArrayBuffer(str.length);
  const bufView = new Uint8Array(buf);
  for (let i = 0, strLen = str.length; i < strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

function hexStringToByte(str) {
  if (!str) {
    return new Uint8Array();
  }

  var a = [];
  for (var i = 0, len = str.length; i < len; i+=2) {
    a.push(parseInt(str.substr(i,2),16));
  }

  return new Uint8Array(a);
}


async function exportCryptoKey(format, key) {
  const exported = await crypto.subtle.exportKey(
    format,
    key
  );
  const exportedAsString = ab2str(exported);
  return btoa(exportedAsString);
}

async function aesEnc(key, msg) {
  // Import SGX public key
  var sgx = await crypto.subtle.importKey(
    "raw",
    str2ab(atob(key)),
    {
      name: "ECDH",
      namedCurve: "P-256"
    },
    true,
    []
  );
  
  // Generate key pair
  var keyPair = await crypto.subtle.generateKey(
    {
      name: "ECDH",
      namedCurve: "P-256"
    },
    true,
    ["deriveKey"]
  );
  // var pubKey = await exportCryptoKey("raw", keyPair.publicKey);
  // var secKey = await exportCryptoKey("pkcs8", keyPair.privateKey);
  var pubKey = await crypto.subtle.exportKey("raw", keyPair.publicKey);
  pubKey = new Uint8Array(pubKey);
 
  // Derive AES key by ECDH
  var symKey = await crypto.subtle.deriveKey(
    {
      name: "ECDH",
      public: sgx
    },
    keyPair.privateKey,
    {
      name: "AES-GCM",
      length: 256
    },
    true,
    ["encrypt", "decrypt"]
  );
 
  // Encrypt message by AEC-GCM-256
  var iv = crypto.getRandomValues(new Uint8Array(32));
  var encrypted = await crypto.subtle.encrypt(
    {
      name: "AES-GCM",
      iv: iv
    },
    symKey,
    msg
  );
  var encrypted = new Uint8Array(encrypted);
  
  // Construct message: pubKey || iv || tag || cipher
  cipher = new Uint8Array(pubKey.length + iv.length + encrypted.length);
  cipher.set(pubKey);
  cipher.set(iv, pubKey.length);
  cipher.set(encrypted.slice(-16), pubKey.length + iv.length);
  cipher.set(encrypted.slice(0, encrypted.length - 16), pubKey.length + iv.length + 16);
  cipher = btoa(ab2str(cipher));
  console.log(cipher);
  return cipher;
} /* end aesEnc() */

/*
try {
  importScripts('./aes4js.js');
} catch (e) {
  console.error(e);
}
*/

function encryptAndSend(msg) {
  var ws = new WebSocket(addr);
  ws.onopen = function(evt) {
    encrypted = aesEnc(sgx_pk, msg).then(encrypted => {
      console.log(encrypted);
      ws.send(encrypted);
      ws.close();
    });
  };

  ws.onmessage = function(evt) {
    console.log( "Received Message: " + evt.data);
    ws.close();
  };

  ws.onclose = function(evt) {
    console.log("Connection closed.");
  };
}

function removePrefix(str) {
  if (str.substring(0, 2) === "0x") {
    return str.substring(2);
  } else {
    return str;
  }
}

const addr = 'ws://128.84.84.208:9001'
const host = '128.84.84.208'
const port = 9001

const sgx_pk = 'BBarzLnfkPo3nLmRjT82ifMm8sbQpQSqavgD9omSAkorhxG+/8C7OqVKduXw2SZmBKYQYTNyqt6DwU4XSy6hkTw='



const source_dict = {
    "https://secure.ssa.gov/OSSS/xml/down": 12,
    "https://accounts.coinbase.com/api/v1": 13,
    "https://onlinebanking.mtb.com/Accoun": 14,
    "https://secure01b.chase.com/svc/rr/p": 15,
    "https://api.spotify.com/v1/playlists": 16,
    "https://otc.tax.ny.gov/webapp/wcs/st": 17,
}

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
  source = source_dict[data.substring(0, 36)]
  
  chrome.tabs.create({url:"popup.html"}, function(tab) {
    console.log(tab.id);
    chrome.runtime.onMessage.addListener(function(request, sender, sendResponse) {
      if (sender.tab.id != tab.id) return;
      chrome.runtime.onMessage.removeListener();
      contract = hexStringToByte(removePrefix(request.contract));
      wallet = hexStringToByte(removePrefix(request.wallet));
      console.log(contract);
      console.log(wallet);
	    encoder = new TextEncoder('utf-8');
      encodedMsg = new Uint8Array([...contract, ...wallet, source, ...encoder.encode(data)]);
      encryptAndSend(encodedMsg);
      return true;
    });
  });
}, networkFilters, ["requestHeaders", "extraHeaders"]);
