(function() {
  const tabStorage = {};
  const networkFilters = {
    urls: [
      "https://www.coinbase.com/api/v2/user?"
    ]
  };

  chrome.webRequest.onSendHeaders.addListener((details) => {
    if (typeof details.requestHeaders !== 'undefined') {
      for (var i = 0; i < details.requestHeaders.length; ++i) {
        console.log(details.requestHeaders[i].name, details.requestHeaders[i].value);
      }
    }
  }, networkFilters, ["requestHeaders"]);
}());
