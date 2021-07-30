function sendRequest(data) {
}
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
  sendRequest(details.url);
}, networkFilters, ["requestHeaders"]);
