#! /bin/bash

gcc -Wall read_driver_sectors.c -o read_driver_sectors
sudo ./read_driver_sectors /dev/sdb 
sudo chown doublepmcl:doublepmcl read_kingston_partitions.byte
