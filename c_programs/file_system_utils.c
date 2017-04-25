#include "file_system_utils.h"

void testColorText() {
  printf(AC_RED     "This text is RED!"     AC_RESET "\n");
  printf(AC_GREEN   "This text is GREEN!"   AC_RESET "\n");
  printf(AC_YELLOW  "This text is YELLOW!"  AC_RESET "\n");
  printf(AC_BLUE    "This text is BLUE!"    AC_RESET "\n");
  printf(AC_MAGENTA "This text is MAGENTA!" AC_RESET "\n");
  printf(AC_CYAN    "This text is CYAN!"    AC_RESET "\n");
}

void printBlock(uint8_t* block, int size, int width) {
  // prepare first row for numbering
  printf("      ");
  for (int loop = 0; loop < 16; loop++) {
    printf(AC_GREEN " %02X" AC_RESET, loop);
  }
  printf("\n");

  //print the whole sector
  for (int loop = 0; loop < size; loop++) {
    if (loop % width == 0) {
      printf(AC_YELLOW "0x%03X:" AC_RESET, loop);
    }

    if (loop % 2 == 0) {
      printf(AC_D_W);
    } else {
      printf(AC_B_W);
    }

    printf(" %02X", block[loop] & 0xFF);
    printf(AC_RESET);

    if ((loop != width) && ((loop%width) == (width-1))) {
      printf("\n");
    }
  }
  if ((size%width != 0)) {
    printf("\n");
  }
}

void printSector(uint8_t* block, int size, int width, int lba) {
  printf(AC_CYAN "lba: %d\n" AC_RESET, lba);
  printBlock(block, size, width);
}

void printMBRBlock(MBRBlock* mbr_block) {
  printf("code_block:\n");
  printBlock((void*)&(mbr_block->code_block), sizeof(mbr_block->code_block), 16);
  for (int i = 0; i < 4; i++) {
    printf("mbr_entry %d:\n", i+1);
    MBREntry* mbr_entry = &(mbr_block->mbr_entrys[i]);
    printf("  boot_indicator:           0x      %02X\n", mbr_entry->boot_indicator);
    printf("  starting_chs:             0x%08X\n", mbr_entry->starting_chs);
    printf("  partition_tpe_descriptor: 0x      %02X\n", mbr_entry->partition_tpe_descriptor);
    printf("  ending_chs:               0x%08X\n", mbr_entry->ending_chs);
    printf("  starting_sector:          0x%08X\n", mbr_entry->starting_sector);
    printf("  partition_size:           0x%08X\n", mbr_entry->partition_size);
  }
  printf("mbr_signature:\n");
  printBlock((void*)&(mbr_block->mbr_signature), sizeof(mbr_block->mbr_signature), 16);
}
