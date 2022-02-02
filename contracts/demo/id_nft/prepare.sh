rm -rf ./build
solc -o ./build/ --bin --abi  @openzeppelin/=../contracts/openzeppelin/ ../contracts/id_nft.sol 

# Make sure you modify the TC contract to have the right SGX address.
SRC=$(paste ./build/IdNFT.abi)

cat <<EOF
var ABI = $SRC
EOF

SRC=$(paste ./build/IdNFT.bin)

cat <<EOF
var BIN = '0x$SRC'
EOF

cat << EOF

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

var IdContract = eth.contract(ABI)

function unlockAccounts() {
    for (var i = 0; i < eth.accounts.length; ++i) {
        personal.unlockAccount(eth.accounts[i], '000000');
    }
}

function mineBlocks(num) {
    miner.start(1); miner.start(1); admin.sleepBlocks(num); miner.stop();
}

function watchEvents(contract) {
    var his = contract.allEvents({fromBlock: 0, toBlock: 'latest'});
    var events;
    his.get(function(error, result) {
        if (!error) {
            console.log(result.length);
            for (var i = 0; i < result.length; ++i) {
                console.log(i + " : " + result[i].event);
            }
            events = result;
        } else {
            console.log("error");
            events = "error";
        }
    });
    return events;
}

function transferOwnership(contract, old_owner, new_owner) {
  unlockAccounts();
  contract.transferOwnership.sendTransaction(new_owner, {
    from: old_owner,
    gas: gasCnt},
    function(e) {
      if (!e) {
        console.log("Owner of the contract: " + new_owner);
      } else {
        console.log('Failed to transfer ownership of contract' + e);
      }
    });
  mineBlocks(1);
}

function setupContract() {
  unlockAccounts();
  var deploy = IdContract.new({
    from: minerAddr,
    data: BIN,
    gas: gasCnt}, 
    function(e, c) {
      if (!e) {
        if (c.address) {
          console.log('IdNFT contract created at: ' + c.address);
          IdNFT = IdContract.at(c.address);
          transferOwnership(IdNFT, minerAddr, sgxAddr);
          return IdNFT;
        }
      } else {
        console.log('Failed to create IdNFT contract: ' + e);
      }
    });
  mineBlocks(1);
}
EOF
