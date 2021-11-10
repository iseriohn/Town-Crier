#!/bin/bash
source env.sh

$GETH --datadir=$DATADIR removedb

$GETH --datadir=$DATADIR \
  --verbosity=$VERBOSITY \
  --keystore=$KEY_STORE \
  init genesis.json

$GETH --datadir=$DATADIR \
  --identity="00" \
  --networkid=$NETWORKID \
  --port=$LISTEN_PORT \
  --verbosity=$VERBOSITY \
  --nodiscover \
  --ipcpath=$IPC_PATH \
  --ws \
  --ws.addr=$WS_ADDR \
  --ws.port=$WS_PORT \
  --ws.api="db,eth,ne,web3,personal,crypto-js" \
  --http \
  --http.addr=$HTTP_ADDR \
  --http.port=$HTTP_PORT \
  --http.api="admin,debug,web3,eth,txpool,personal,miner,net,crypto-js" \
  --http.vhosts '*' \
  --http.corsdomain '*' \
  --allow-insecure-unlock \
  --keystore=$KEY_STORE \
  --vmdebug \
  console
