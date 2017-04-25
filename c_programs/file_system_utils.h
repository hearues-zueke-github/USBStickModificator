#ifndef FILE_SYSTEM_UTILS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define SECTOR_SIZE 0x200 // 512

#define AC_RED_BOLD "\x1b[1;49;31m"

#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_BLUE    "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN    "\x1b[36m"
#define AC_D_W     "\x1b[37m"
#define AC_B_W     "\x1b[1;37m"
#define AC_RESET   "\x1b[0m"

typedef struct MBREntry_ {
    uint8_t boot_indicator;
    uint32_t starting_chs:24;
    uint8_t partition_tpe_descriptor;
    uint32_t ending_chs:24;
    uint32_t starting_sector;
    uint32_t partition_size;
} __attribute__((packed)) MBREntry;

typedef struct MBREntry_ MBREntry;

typedef struct MBRBlock_ {
    uint8_t code_block[0x1BE];
    MBREntry mbr_entrys[4];
    uint8_t mbr_signature[2];
} __attribute__((packed)) MBRBlock;

void testColorText();
void printBlock(uint8_t* block, int size, int width);
void printSector(uint8_t* block, int size, int width, int lba);
void printMBRBlock(MBRBlock* mbr_block);

#endif /* FILE_SYSTEM_UTILS */
