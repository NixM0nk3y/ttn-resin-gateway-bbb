#!/bin/bash
if [ ! -e /dev/spidev1.0 ]; then
  config-pin overlay BB-SPIDEV0
fi

/etc/node_exporter --web.listen-address "127.0.0.1:81" &

export PATH=$PATH:/opt/gwexporter/bin
node /opt/gwexporter/gwexporter.js &

python /opt/ttn-gateway/run.py
