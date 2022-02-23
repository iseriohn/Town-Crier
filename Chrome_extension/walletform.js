document.getElementById("myButton").addEventListener("click", sendInput);


function sendInput() {
  var walletAddr = document.getElementById("addressInput").value;
  if (walletAddr.startsWith("0x") && walletAddr.length == 42) {
    document.getElementById("status").innerHTML = "Awaiting response... If you don't see response from SGX in 5s, try refreshing the extension and the loading the profile page again.";
    chrome.runtime.sendMessage({wallet: walletAddr});
  } else {
    document.getElementById("status").innerHTML = "Enter a valid address, 0x...";
  }
}

chrome.runtime.onMessage.addListener(
  function(request, sender, sendResponse) {
    document.getElementById("signature").innerHTML = "Response from SGX: " + request;
  }
);
