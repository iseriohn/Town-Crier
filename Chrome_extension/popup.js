document.getElementById("myButton").addEventListener("click", sendInput);


function sendInput() {
  var walletAddr = document.getElementById("addressInput").value;
  if (walletAddr.startsWith("0x") && walletAddr.length == 42) {
    document.getElementById("status").innerHTML = "Request for Identity NFT sent!";
    chrome.runtime.sendMessage({wallet: walletAddr});
  } else {
    document.getElementById("status").innerHTML = "Enter a valid address, 0x...";
  }
}

chrome.runtime.onMessage.addListener(
  function(request, sender, sendResponse) {
    document.getElementById("signature").innerHTML = request;
  }
);