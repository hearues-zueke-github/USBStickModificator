#include "read_driver_sectors.h"

void testColorText() {
  printf(AC_RED     "This text is RED!"     AC_RESET "\n");
  printf(AC_GREEN   "This text is GREEN!"   AC_RESET "\n");
  printf(AC_YELLOW  "This text is YELLOW!"  AC_RESET "\n");
  printf(AC_BLUE    "This text is BLUE!"    AC_RESET "\n");
  printf(AC_MAGENTA "This text is MAGENTA!" AC_RESET "\n");
  printf(AC_CYAN    "This text is CYAN!"    AC_RESET "\n");
}

void printBlock(uint8_t* block, int size, int width, int lab) {
  // print other arguments
  printf(AC_CYAN "LAB = %d\n" AC_RESET, lab);

  // prepare first row for numbering
  printf("      ");
  for (int loop = 0; loop < 16; loop++) {
    printf(AC_GREEN " %2X" AC_RESET, loop);
  }
  printf("\n");

  // print the whole sector
  for (int loop = 0; loop < size; loop++) {
    if (loop % width == 0) {
      printf(AC_YELLOW "0x%3X:" AC_RESET, loop);
    }

    if (loop % 2 == 0) {
      printf(AC_D_W);
    } else {
      printf(AC_B_W);
    }

    printf(" %02X", block[loop] & 0xFF);
    printf(AC_RESET);

    if (loop != width && loop % width == width - 1) {
      printf("\n");
    }
  }
}

int main(int argc, char* argv[])
{
  char file_path[] = "/dev/sdb";
  FILE* f_driver = fopen(file_path, "rb");
  system("mkdir -p sector");
  system("cd sector");
  if (f_driver == 0)
  {
    printf("Can't open %s!\nreturn %d\n", file_path, -1);
  }

  uint8_t block[SECTOR_SIZE];
  uint8_t zero_block[SECTOR_SIZE];
  memset(zero_block, 0, SECTOR_SIZE);
  

  char command[128];
  for (int cluster = 0; cluster < 16; cluster++) {
    for (int lba = 0; lba < 1024; lba++) {
      char foder_cluster_name[30] = {0};
      sprintf(foder_cluster_name, "cluster_0x%08X", cluster);

      sprintf(command, "mkdir -p %s", foder_cluster_name);
      system(command);
      sprintf(command, "cd %s", foder_cluster_name);
      system(command);

      fseek (f_driver, (cluster*16+lba)*SECTOR_SIZE, SEEK_SET);

      int read = fread(block, sizeof(uint8_t), SECTOR_SIZE, f_driver);
      if (!read) {
        printf("Failed to read in!\n");
        return -1;
      }
      
      char file_path_out[30] = {0};
      char* c = file_path_out;
      // sprintf(c, "%s", "sector/");
      // for (; *c != 0; c++) {}
      sprintf(c, "0x%07X.txt", lba);
      // for (; *c != 0; c++) {}
      // sprintf(c, "%s", ".txt");

      if (memcmp(zero_block, block, SECTOR_SIZE) == 0) {
        continue;
      }

      FILE* fout = fopen(file_path_out, "wb");
      fwrite(block, sizeof(char), sizeof(block), fout);
      fclose(fout);

      sprintf(command, "cd ..");
      system(command);
    }
  }

  fclose(f_driver);

  return 0;
}
