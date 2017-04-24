#! /bin/bash

make
sudo ./read_driver_sectors
sudo chown -cR doublepmcl:doublepmcl sector/
