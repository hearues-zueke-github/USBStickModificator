#include "file_system_utils.h"

int main(int argc, char* argv[])
{
  char file_path[] = "/dev/sdb";
  FILE* f_driver = fopen(file_path, "rb");
  system("mkdir -p sector");
  system("cd sector");
  if (f_driver == NULL)
  {
    printf("Can't open %s!\nreturn %d\n", file_path, -1);
  }

  printf("sizeof(MBREntry): %lu\n", sizeof(MBREntry));
  printf("sizeof(MBRBlock): %lu\n", sizeof(MBRBlock));

  MBRBlock mbr_block;
  fseek(f_driver, 0, SEEK_SET);
  fread(&mbr_block, 1, SECTOR_SIZE, f_driver);
  printMBRBlock(&mbr_block);

  uint8_t block[SECTOR_SIZE];
  uint8_t zero_block[SECTOR_SIZE];
  memset(zero_block, 0, SECTOR_SIZE);
  
  fseek(f_driver, 0, SEEK_SET);
  fread(block, 1, SECTOR_SIZE, f_driver);
  printBlock(block, SECTOR_SIZE, 16);

  fclose(f_driver);

  FILE* f_driver_out = fopen(file_path, "wb");
  fseek(f_driver_out, 0x1BE, SEEK_SET);
  for (int i = 0; i < 0x40; i++) {
      block[i] = i+1;
  }
  fwrite(block, 1, 0x40, f_driver_out);
  fclose(f_driver_out);

  FILE* f_mbr_sector = fopen("mbr_sector.hex", "wb");
  fwrite(block, 1, SECTOR_SIZE, f_mbr_sector);
  fclose(f_mbr_sector);
  
  exit(0);

  char command[1024];
  sprintf(command, "mkdir -p sector");
  system(command);

  char path[1024];
  getcwd(path, sizeof(path));
  printf("1: path: %s\n", path);
  
  char* cp;
  cp = path;
  for (; cp[0] != 0; cp++);
  sprintf(cp, "/sector");
  printf("2: path: %s\n", path);
  chdir(path);

  for (int cluster = 0; cluster < 16; cluster++) {
    char folder_cluster_name[30] = {0};
    sprintf(folder_cluster_name, "cluster_0x%08X", cluster);

    sprintf(command, "mkdir -p %s", folder_cluster_name);
    system(command);

    for (; cp[0] != 0; cp++);
    sprintf(cp, "/%s", folder_cluster_name);
    printf("3: path: %s\n", path);
    chdir(path);
    exit(0);
    
    for (int lba = 0; lba < 1024; lba++) {
      fseek (f_driver, (cluster*16+lba)*SECTOR_SIZE, SEEK_SET);

      int read = fread(block, sizeof(uint8_t), SECTOR_SIZE, f_driver);
      if (!read) {
        printf("Failed to read in!\n");
        return -1;
      }

      if (memcmp(zero_block, block, SECTOR_SIZE) == 0) {
        continue;
      }
      
      char fout_name[30] = {0};
      sprintf(file_path, "0x%07X.txt", lba);

      FILE* fout = fopen(fout_name, "wb");
      if (fout == NULL) {
        printf("No file %s open!\n", fout_name);
        continue;
      }
      fwrite(block, sizeof(char), sizeof(block), fout);
      fclose(fout);
    }
    
    for (; *cp != 0; cp++);
    for (; *cp != '/'; cp--) { *cp = 0; }
    *cp = 0;
    printf("path: %s\n", path);
    chdir(path);
  }

  fclose(f_driver);

  return 0;
}
