var contract = window.prompt("What is the address of the Identity NFT contract?");
var wallet = window.prompt("What is your wallet address for receiving the Identity NFT?")

chrome.runtime.sendMessage({contract: contract, wallet: wallet});

chrome.runtime.onMessage.addListener(
  function(request, sender, sendResponse) {
    document.getElementById("signature").innerHTML = request;
  }
);

