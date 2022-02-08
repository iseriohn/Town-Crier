const ethers = require("ethers")
const keythereum = require("keythereum-node");

address = "0x5C7360D0006534030118F93132c354E0dE6971e5"
datadir = "/home/yan/Documents/Town-Crier/contracts/demo/id_nft/chain0/"
password = "000000"

keythereum.importFromFile(address, datadir)
.then(keyObject => {
  keythereum.recover(password, keyObject)
  .then(privateKey => {
    privkey = privateKey.toString('hex');
    provider = new ethers.providers.JsonRpcProvider("http://localhost:8646");
    wallet = new ethers.Wallet(privkey, provider);
    wallet.signMessage("hi").then(sig => {
     console.log(sig);
    });
  });
});

