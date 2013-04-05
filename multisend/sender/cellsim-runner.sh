#!/bin/bash

exec /home/ravinet/mininet/mininet/multisend/sender/cellsim /home/ravinet/mininet/mininet/verizon_lte_uplink_slice.ms /home/ravinet/mininet/mininet/verizon_lte_downlink_slice.ms 00:00:00:00:00:02 0 LTE-eth0 LTE-eth1 >/tmp/cellsim-stdout 2>/tmp/cellsim-stderr
