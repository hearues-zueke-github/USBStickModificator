#! /usr/bin/python3.5

import numpy as np
import Utils as utils

SEEK_SET = 0
SECTOR_SIZE = 0x200 # 512

index = 0x0

dev_path = "/dev/sdb"

# WARNING: DO NOT DO THIS TO YOUR SYSTEM/USB-DRIVE/USB-STICK!!!!!!!!!
zero_sector = np.zeros(SECTOR_SIZE).astype(np.uint8)
with open(dev_path, "wb") as f:
    f.seek(SECTOR_SIZE*index, SEEK_SET)
    f.write(zero_sector)

with open(dev_path, "rb") as f:
    f.seek(SECTOR_SIZE*index, 0)
    read_sector = np.fromfile(f, dtype=np.uint8, count=SECTOR_SIZE)

utils.pretty_block_printer(read_sector, 8, SECTOR_SIZE)
