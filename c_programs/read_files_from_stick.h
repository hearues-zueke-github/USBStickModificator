#ifndef READ_FILES_FROM_STICK

#define INPUT_LENGTH 1024
#define COMMANDS 10

#include "file_system_utils.h"

int read_input(char* input, char** commands, int* idx);
void get_normal_input(int idx, char* input, char* input_copy, char** commands);
int is_number_decimal(char* c);
int is_number_hex(char* c);

#endif /* READ_FILES_FROM_STICK */
