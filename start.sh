#!/bin/bash
if [ ! -e /dev/spidev1.0 ]; then
  config-pin overlay BB-SPIDEV0
fi
python /opt/ttn-gateway/run.py
