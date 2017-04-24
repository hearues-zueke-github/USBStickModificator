#ifndef READ_DRIVER_SECTORS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SECTOR_SIZE 0x200 // 512

#define AC_RED     "\x1b[31m"
#define AC_GREEN   "\x1b[32m"
#define AC_YELLOW  "\x1b[33m"
#define AC_BLUE    "\x1b[34m"
#define AC_MAGENTA "\x1b[35m"
#define AC_CYAN    "\x1b[36m"
#define AC_D_W     "\x1b[37m"
#define AC_B_W     "\x1b[1;37m"
#define AC_RESET   "\x1b[0m"

void testColorText();
void printBlock(uint8_t* block, int size, int width, int lab);

#endif /* READ_DRIVER_SECTORS_H */
