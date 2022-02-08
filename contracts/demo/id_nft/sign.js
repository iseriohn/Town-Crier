
if (eth.accounts.length < 3)
{
    console.log(eth.accounts.length + ' accounts found. Need 3')
    exit;
}

var minerAddr = eth.accounts[0]
var AliceAddr = eth.accounts[1]
var BobAddr = eth.accounts[2]
var sgxAddr = "0x89b44e4d3c81ede05d0f5de8d1a68f754d73d997"

var gasCnt = 3e+8
var TC_FEE = 3e+15


function unlockAccounts() {
    for (var i = 0; i < eth.accounts.length; ++i) {
        personal.unlockAccount(eth.accounts[i], '000000');
    }
};

function mineBlocks(num) {
    miner.start(1); miner.start(1); admin.sleepBlocks(num); miner.stop();
}

function hexStringToByteArray(hexString) {
      if (hexString.length % 2 !== 0) {
                throw "Must have an even number of hex digits to convert to bytes";
            }/* w w w.  jav  a2 s .  c o  m*/
      var numBytes = hexString.length / 2;
      var byteArray = new Uint8Array(numBytes);
      for (var i=0; i<numBytes; i++) {
                byteArray[i] = parseInt(hexString.substr(i*2, 2), 16);
            }
      return byteArray;
}

