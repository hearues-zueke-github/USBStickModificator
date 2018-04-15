#! /usr/bin/python3.5

import os
import sys

import numpy as np

from Utils import pretty_block_printer

SEEK_SET = 0
SECTOR_SIZE = 0x200 # 512

index = 0x0

argv = sys.argv

if len(argv) < 2:
    print("No file/path defined")
    sys.exit(-1)

dev_path = sys.argv[1]



while True:
    inp = input("> ").split()
    print("len(inp): {}".format(len(inp)))
    print("inp: {}".format(inp))

    if len(inp) == 2:
        if inp[0] == "gs":
            index = int(inp[1])

            with open(dev_path, "rb") as f:
                f.seek(SECTOR_SIZE*index, 0)
                read_sector = np.fromfile(f, dtype=np.uint8, count=SECTOR_SIZE)

            pretty_block_printer(read_sector, 8, SECTOR_SIZE)
        elif inp[0] == "ss":
            index = int(inp[1])

            rnd_data = np.random.randint(0, 256, (SECTOR_SIZE, )).astype(np.uint8)
            with open(dev_path, "wb") as f:
                f.seek(SECTOR_SIZE*index, 0)
                f.write(rnd_data)

            print("rnd_data:")
            pretty_block_printer(rnd_data, 8, SECTOR_SIZE)

# WARNING: DO NOT DO THIS TO YOUR SYSTEM/USB-DRIVE/USB-STICK!!!!!!!!!
# zero_sector = np.zeros(SECTOR_SIZE).astype(np.uint8)
# with open(dev_path, "wb") as f:
#     f.seek(SECTOR_SIZE*index, SEEK_SET)
#     # f.write(zero_sector)
#     f.write(np.random.randint(0, 256, (SECTOR_SIZE, )).astype(np.uint8))

