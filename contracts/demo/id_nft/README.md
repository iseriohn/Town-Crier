Testing
=======

This document tells you how to test Town Crier on a private Ethereum net.
First of all, a version of geth (>= 1.6) should be installed.

    sudo add-apt-repository -y ppa:ethereum/ethereum
    sudo apt-get update
    sudo apt-get install ethereum solc

1. Compile smart contracts by `./prepare.sh > demorc.js`.
   Then you can modify `demorc.js`, but REMEMBER to paste the updated `demorc.js` in `prepare.sh`!!!

2. Spin up `geth` by running `./geth_privnet.sh`.

3. `geth` will reuse keys stored in `wallet_privnet`. 
    To make sure your geth has picked them up, run the following command within the geth console and compare the output verbatim.
    Also, verify in `genesis.json`, same accounts are allocated with ethers.

4. You may ssh tunnel to the geth node on a remote server by `ssh -L 8545:127.0.0.1:8646 [remote_server]`. 
   Then you can set the Metamask/Remix RPC addr to 127.0.0.1:8545 to connect to the geth node.

5. In geth console, load `demorc.js` and setup the NFT contract (follow the commands at the end of `demorc.js`).
        
        // For privatenet:
        // loadScript("scripts/demorc.js");
        // IdNFT = setupContract();
        // (or if there is already an IdNFT) IdNFT = IdContract.at("contract address from receipt");
        
        // setup_log(tc, app, 0);
        // watch_events(tc);
        // watch_events(app);
        // Request(app, 1, ['FJM273', pad(1492100100, 64)]);
        // Request(app, 2, ['f68d2a32cf17b1312c6db3f236a38c94', '4c9f92f6ec1e2a20a1413d0ac1b867a3', '32884794', pad(1456380265, 64), pad(1, 64), 'Portal']);
        // Request(app, 3, ['GOOG', pad(1262390400,64)]);
        // Request(app, 4, ['1ZE331480394808282']);
        // Request(app, 5, ['bitcoin']);

6. Change the contract address in `Town-Crier/config/config-privatenet-sim` and rebuild TC.

7. Launch TC.

8. On the user side, turn on the Chrome extension and run proxy.py with wallet address.

9. Login to SSA and Coinbase to obtain Identity NFTs.

