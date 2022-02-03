//Contract based on [https://docs.openzeppelin.com/contracts/3.x/erc721](https://docs.openzeppelin.com/contracts/3.x/erc721)
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/token/ERC721/ERC721.sol";
import "@openzeppelin/contracts/utils/Counters.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

contract IdNFT is ERC721, Ownable {
    using Counters for Counters.Counter;
    Counters.Counter private _tokenIds;
    address sgx = 0x89B44e4d3c81EDE05D0f5de8d1a68F754D73d997; // SGX address

    constructor() ERC721("IdNFT", "NFT") {}

    function mintNFT(uint8 v, bytes32 r, bytes32 s)
        public 
        returns (uint256)
    {
        //bytes memory data = abi.encodePacked(address(this), msg.sender);
        bytes memory data = abi.encodePacked("hi");
        bytes32 hash = keccak256(abi.encodePacked("\x19Ethereum Signed Message:\n32", data));
        
        require(ecrecover(hash, v, r, s) == sgx);
        require(balanceOf(msg.sender) == 0);

        _tokenIds.increment();
        uint256 newItemId = _tokenIds.current();
        _mint(msg.sender, newItemId);

        return newItemId;
    }
}

