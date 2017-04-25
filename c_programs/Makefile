CC=gcc
CFLAGS=-Wall -std=c99
DEPS=-lrt -pthread

all:
	rm -f read_driver_sectors
	rm -f read_files_from_stick
	$(CC) $(CFLAGS) read_driver_sectors.c file_system_utils.c -o read_driver_sectors
	$(CC) $(CFLAGS) read_files_from_stick.c file_system_utils.c -o read_files_from_stick
