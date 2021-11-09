#!/bin/bash

# where things are
export GETH=geth
export MIST=~/bin/mist/mist

# geth configuration
export NETWORKID=23
export LISTEN_PORT=30000
export WS_PORT=8646
export WS_ADDR=127.0.0.1
export HTTP_PORT=8646
export HTTP_ADDR=127.0.0.1

export IPC_PATH=~/.ethereum/geth.ipc
export DATADIR=chain0
export KEY_STORE=wallet_privnet

# testnet configuration
export TESTNET_DATADIR=~/.ethereum/testnet

export MAINNET_DATADIR=~/.ethereum

# verbosity
export VERBOSITY=2
export MIST_LOG_LVL=error

mkdir -p $DATADIR
mkdir -p $TESTNET_DATADIR
