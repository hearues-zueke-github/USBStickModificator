#ifndef READ_FILES_FROM_STICK

#define INPUT_LENGTH 1024
#define COMMANDS 10

#include "file_system_utils.h"

typedef enum {
    EXIT,
    NEWPROGRAM,
    DEFINES,
    SETFILE,
    DRIVER,
    SETSUDO,
    GETSECTOR,
    WRITEAT,
    SETZEROSECTOR,
    SETONESSECTOR,
    GETFILE_ZERO_ONES_SECTORS,
    PRINTARGS,
    CD,
    HELP,
    BASH
} CommandNumber;

int read_input(char* input, char** commands, int* idx);
void get_normal_input(int idx, char* input, char* input_copy, char** commands);
int is_number_decimal(char* c);
int is_number_hex(char* c);

int read_input(char* input, char** commands, int* idx);
void print_error_message(int error_ret);
void get_normal_input(int idx, char* input, char* input_copy, char** commands);

CommandNumber get_command_number(char* command_0);
int is_number_decimal(char* c);
int is_number_hex(char* c);

int convert_str_to_int(int* num, char* str_num);

void command_newprogram();
void command_defines();
void command_setfiles(char** commands, int idx);
void command_driver(char** commands, int idx);
void command_setsudo(char** commands, int idx);
void command_getsector(char** commands, int idx);
void command_writeat(char** commands, int idx);
void command_setzerosector(char** commands, int idx);
void command_setonessector(char** commands, int idx);
void command_getfile_zero_ones_sectors();
void command_printargs(int argc, char** argv);
void command_cd(char** commands, int idx);
void command_print();

#endif /* READ_FILES_FROM_STICK */
