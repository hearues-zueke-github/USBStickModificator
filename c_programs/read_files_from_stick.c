#include "read_files_from_stick.h"

char current_path[1024];
char file_path[1024];

int read_input(char* input, char** commands, int* idx) {
    memset(input, 0, INPUT_LENGTH);
    int length = 0;
    int is_last_space = 0;
    char* c = input;
    commands[0] = c;
    // Get the whole input from the console
    *idx = 1;
    while ((*c = getchar()) == ' ');
    if (*c == '\n') {
        *c = 0;
        return 3;
    }
    c++;
    length++;
    while ((*c = getchar()) != '\n') {
        length++;
        if (length >= INPUT_LENGTH) {
            while ((*c = getchar()) != '\n');
            return 1;
        }
        if (*c == ' ') {
            *c = 0;
            if (!is_last_space) {
                c++;
            }
            is_last_space = 1;
        } else if ((*c != ' ') && is_last_space) {
            commands[*idx] = c;
            (*idx)++;
            if (*idx > COMMANDS) {
                while ((*c = getchar()) != '\n');
                return 2;
            }
            is_last_space = 0;
        }
        if (!is_last_space) {
            c++;
        }
    }

    *c = 0;

    return 0;
}

void print_error_message(int error_ret) {
    switch (error_ret) {
        case 1:
            printf(AC_RED_BOLD "Error:" AC_RESET " max char length: %d\n", INPUT_LENGTH);
            break;
        case 2:
            printf(AC_RED_BOLD "Error:" AC_RESET " not more than %d commands!\n", COMMANDS);
            break;
        case 3:
            break;
    }

}

void get_normal_input(int idx, char* input, char* input_copy, char** commands) {
    if (idx == 0) {
        *input_copy = 0;
        return;
    }
    char* c = commands[idx-1];
    while (*c != 0) { c++; }
    memset(input_copy, 0, INPUT_LENGTH);
    memcpy(input_copy, input, (size_t)c-(size_t)input);
    char* c1 = input_copy+(int)(c-input);
    do {
        c1--;
        if (*c1 == 0) {
            *c1 = ' ';
        }
    } while (c1 != input_copy);
}

CommandNumber get_command_number(char* command_0) {
    if (!strcmp(command_0, "exit") || !strcmp(command_0, "e")) {
        return EXIT;
    } else if (!strcmp(command_0, "newprogram") || !strcmp(command_0, "n")) {
        return NEWPROGRAM;
    } else if (!strcmp(command_0, "defines")) {
        return DEFINES;
    } else if (!strcmp(command_0, "setfile")) {
        return SETFILE;
    } else if (!strcmp(command_0, "driver")) {
        return DRIVER;
    } else if (!strcmp(command_0, "getsector") || !strcmp(command_0, "gsec")) {
        return GETSECTOR;
    } else if (!strcmp(command_0, "setsudo") || !strcmp(command_0, "ssudo")) {
        return SETSUDO;
    } else if (!strcmp(command_0, "writeat") || !strcmp(command_0, "w")) {
        return WRITEAT;
    } else if (!strcmp(command_0, "setzerosector") || !strcmp(command_0, "szs")) {
        return SETZEROSECTOR;
    } else if (!strcmp(command_0, "setonessector") || !strcmp(command_0, "sos")) {
        return SETONESSECTOR;
    } else if (!strcmp(command_0, "getfilezeroonessectors") || !strcmp(command_0, "gfzos")) {
        return GETFILE_ZERO_ONES_SECTORS;
    } else if (!strcmp(command_0, "printargs")) {
        return PRINTARGS;
    } else if (!strcmp(command_0, "cd")) {
        return CD;
    } else if (!strcmp(command_0, "help") || !strcmp(command_0, "h")) {
        return HELP;
    }

    return BASH;
}

int is_number_decimal(char* c) {
    for (; *c != 0; c++) {
        if ((*c < '0') || (*c > '9')) {
            return 0;
        }
    }
    return 1;
}

int is_number_hex(char* c) {
    if (memcmp(c, "0x", 2)) {
        return 0;
    }
    c += 2;
    for (; *c != 0; c++) {
        if (!((('0' <= *c) && (*c <= '9')) ||
              (('A' <= *c) && (*c <= 'F')) ||
              (('a' <= *c) && (*c <= 'f')))) {
            return 0;
        }
    }
    return 1;
}

int convert_str_to_int(int* num, char* str_num) {
    if (is_number_decimal(str_num)) {
        *num = strtol(str_num, NULL, 10);
        return 0;
    } else if (is_number_hex(str_num)) {
        *num = strtol(str_num, NULL, 16);
        return 0;
    }

    printf(AC_RED_BOLD "Error:" AC_RESET " number is not a decimal or hex number!\n");
    return -1;
}

void command_newprogram() {
    system("make");
    char program_name[] = "./read_files_from_stick";
    execl(program_name, program_name, NULL);
}

void command_defines() {
    printf("INPUT_LENGTH: %d\n", INPUT_LENGTH);
    printf("COMMANDS    : %d\n", COMMANDS);
}

void command_setfiles(char** commands, int idx) {
    if (idx < 2) {
        printf(AC_RED_BOLD "Error:" AC_RESET " need 1 argument: " AC_GREEN "file_path" AC_RESET "\n");
    }
    sprintf(file_path, "%s", commands[1]);
}

void command_driver(char** commands, int idx) {
    if (idx < 2) {
        printf(AC_RED_BOLD "Error:" AC_RESET " need 1 argument: " AC_GREEN "driver_name" AC_RESET "\n");
    }
    sprintf(file_path, "/dev/%s", commands[1]);
}

void command_setsudo(char** commands, int idx) {
    // TODO: do this check everywhere, before open a file!
    if (access(file_path, F_OK)) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    char* username = getlogin();
    char bash_command[1024];
    sprintf(bash_command, "sudo chown %s:%s %s", username, username, file_path);
    system(bash_command);
    printf("File " AC_GREEN "%s" AC_RESET " successfull given rights to user " AC_GREEN "%s" AC_RESET "\n", file_path, username);
}

void command_getsector(char** commands, int idx) {
    int lba = 0;
    // TODO: check if idx > 1
    if (is_number_decimal(commands[1])) {
        lba = strtol(commands[1], NULL, 10);
    } else if (is_number_hex(commands[1])) {
        lba = strtol(commands[1], NULL, 16);
    } else {
        printf(AC_RED_BOLD "Error:" AC_RESET " 1st argument is not a decimal or hex number!\n");
        return;
    }

    FILE* f_drive = fopen(file_path, "rb");
    if (f_drive == NULL) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    uint8_t block[SECTOR_SIZE];
    fseek(f_drive, SECTOR_SIZE*lba, SEEK_SET);
    fread((void*)block, 1, SECTOR_SIZE, f_drive);
    fclose(f_drive);
    printSector(block, SECTOR_SIZE, 16, lba);
}

void command_writeat(char** commands, int idx) {
    if (idx < 3) {
        printf(AC_RED_BOLD "Error:" AC_RESET " 2 arguments needed! (first: pos, second: byte value)\n"); 
        return;
    }

    int sector = 0;
    int pos = 0;
    uint8_t byte = 0;

    int is_with_sector = 0;
    commands++;
    if (!strcmp(*commands, "-s") || !strcmp(*commands, "--sector")) {
        if (idx < 5) {
            printf(AC_RED_BOLD "Error:" AC_RESET " with sector 4 arguments needed, e.g.: " AC_YELLOW "writeat s 0x800 (0x000-0x1FF) (0x00-0xFF)" AC_RESET "\n");
            return;
        }
        is_with_sector = 1;

        commands++;
        if (convert_str_to_int(&sector, *commands)) {
            return;
        }
        commands++;
        if (convert_str_to_int(&pos, *commands)) {
            return;
        }
        pos = pos % SECTOR_SIZE;
    } else {
        if (convert_str_to_int(&pos, *commands)) {
            return;
        }
        sector = pos / SECTOR_SIZE;
        pos = pos % SECTOR_SIZE;
    }

    commands++;
    if (!is_number_hex(*commands)) {
        printf(AC_RED_BOLD "Error:" AC_RESET " given bytes should be in hex!\n");
        return;
    }

    FILE* f_drive = fopen(file_path, "wb");
    if (f_drive == NULL) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    fseek(f_drive, sector*SECTOR_SIZE+pos, SEEK_SET);
    int true_length = strlen(*commands)-2;
    int len_of_bytes = (strlen(*commands)-2+1)/2;
    for (int i = 0; i < len_of_bytes; i++) {
        char char_nums[5] = "0x";
        strncpy(char_nums+2, *commands+2+2*i, ((i == len_of_bytes-1) && (true_length%2==1)?1:2));
        byte = strtol(char_nums, NULL, 16);
        fwrite((void*)&byte, 1, 1, f_drive);
    }
    fclose(f_drive);
    if (!is_with_sector) {
        printf("Successfully written at pos " AC_GREEN "0x%X" AC_RESET " bytes " AC_GREEN "%s" AC_RESET " in file " AC_GREEN "%s" AC_RESET "\n", pos, *commands, file_path);
    } else {
        printf("Successfully written at sector " AC_GREEN "0x%X" AC_RESET " pos " AC_GREEN "0x%X" AC_RESET " bytes " AC_GREEN "%s" AC_RESET " in file " AC_GREEN "%s" AC_RESET "\n", sector, pos, *commands, file_path);
    }
}

void command_setzerosector(char** commands, int idx) {
    int lba = 0;
    commands++;
    if (idx > 1) {
        if (convert_str_to_int(&lba, *commands)) {
            return;
        }
    }

    FILE* f_drive = fopen(file_path, "wb");
    if (f_drive == NULL) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    uint8_t block[SECTOR_SIZE];
    memset(block, 0x00, SECTOR_SIZE);
    fseek(f_drive, SECTOR_SIZE*lba, SEEK_SET);
    fwrite((void*)block, 1, SECTOR_SIZE, f_drive);
    fclose(f_drive);
    printf("Successfully write all zeros for sector "AC_GREEN "0x%X" AC_RESET "\n", lba);
}

void command_setonessector(char** commands, int idx) {
    int lba = 0;
    commands++;
    if (idx > 1) {
        if (convert_str_to_int(&lba, *commands)) {
            return;
        }
    }

    FILE* f_drive = fopen(file_path, "wb");
    if (f_drive == NULL) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    uint8_t block[SECTOR_SIZE];
    memset(block, 0xFF, SECTOR_SIZE);
    fseek(f_drive, SECTOR_SIZE*lba, SEEK_SET);
    fwrite((void*)block, 1, SECTOR_SIZE, f_drive);
    fclose(f_drive);
    printf("Successfully write all ones for sector "AC_GREEN "0x%X" AC_RESET "\n", lba);
}

void command_getfile_zero_ones_sectors() {
    printf("file_path: %s\n", file_path);
    FILE* f_drive = fopen(file_path, "rb");
    if (f_drive == NULL) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }

    char f_name[] = "zero_ones_sectors.txt";
    FILE* f_zero_ones_sectors = fopen(f_name, "wb");

    uint8_t block[SECTOR_SIZE];
    uint8_t zero_block[SECTOR_SIZE];
    uint8_t ones_block[SECTOR_SIZE];

    memset(zero_block, 0x00, SECTOR_SIZE);
    memset(ones_block, 0xFF, SECTOR_SIZE);

    int lba = 0;

    sprintf((void*)block, "Write either 0 (for a zero sector) or 1 (for an ones sector) or 2 (for something else)\n");
    fwrite((void*)block, 1, strlen((void*)block), f_zero_ones_sectors);
    char byte = 0;
    (void)byte;
    fseek(f_drive, 0, SEEK_SET);
    while (1) {
        // fseek(f_drive, SECTOR_SIZE*lba, SEEK_SET);
        int ret = fread((void*)block, 1, SECTOR_SIZE, f_drive);
        printf("ret = %d, lba: %d\n", ret, lba);
        if ((ret != SECTOR_SIZE)) { // || (lba >= 32)) {
            break;
        }
        // printBlock(block, SECTOR_SIZE, 16);
        if (!memcmp(block, zero_block, SECTOR_SIZE)) {
            byte = '0';
        } else if (!memcmp(block, ones_block, SECTOR_SIZE)) {
            byte = '1';
        } else {
            byte = '2';
        }
        fwrite(&byte, 1, 1, f_zero_ones_sectors);
        lba++;
        if (lba % 32 == 0) {
            byte = '\n';
            fwrite(&byte, 1, 1, f_zero_ones_sectors);
        }
    }

    fclose(f_drive);
    fclose(f_zero_ones_sectors);

    printf("Successfully write file "AC_GREEN "%s" AC_RESET "\n", f_name);
}

void command_printargs(int argc, char** argv) {
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
}

void command_cd(char** commands, int idx) {
    if (idx < 2) {
        printf(AC_RED_BOLD "Error:" AC_RESET " command " AC_GREEN "cd" AC_RESET " takes one argument\n");
    }
    if (!strcmp(commands[1], ".")) {
        return;
    } else if (!strcmp(commands[1], "..")) {
        char* c = current_path;
        while (*c != 0) { c++; }

        while (*c != '/') { *c = 0; c--; }
        if (c != current_path) {
            *c = 0;
        }
        chdir(current_path);
    } else {
        char* c = current_path;
        while (*c != 0) { c++; }
        char* save_pos = c;
        if (c-1 != current_path) {
            *c = '/';
            c++;
        }
        sprintf(c, "%s", commands[1]);
        if (chdir(current_path)) {
            printf(AC_RED_BOLD "Error:" AC_RESET " folder %s does not exist\n", commands[1]);
            while (*c != 0) { c++; }
            while (c != save_pos) { *c = 0; c--; } *c = 0;
        }
    }
}

void command_print() {
    printf(AC_B_W "commands:\n" AC_RESET);
    printf(AC_GREEN "defines           " AC_RESET "Print all given defines\n");
    printf(AC_GREEN "printargs         " AC_RESET "Print the given argc and argv parameters\n");
    printf(AC_GREEN "setfile           " AC_RESET "Set the path to the file (absolute)\n");
    printf(AC_GREEN "driver            " AC_RESET "Set the path to the file (" AC_CYAN "/dev/" AC_RESET " as preffix)\n");
    printf(AC_GREEN "getsector, gsec   " AC_RESET "Prints the sector block with the given " AC_CYAN "index" AC_RESET " ("AC_YELLOW"decimal"AC_RESET" or "AC_YELLOW"hex"AC_RESET" value)\n");
    printf(AC_GREEN "writeat, w        " AC_RESET "Write some bytes at " AC_CYAN "absolute_pos " AC_RESET "("AC_YELLOW"decimal"AC_RESET" or "AC_YELLOW"hex"AC_RESET") or with "AC_CYAN"(-s|--sector)"AC_RESET" set first sector and\n"
                    "                  then " AC_CYAN "pos " AC_RESET "(both "AC_YELLOW"decimal"AC_RESET" or "AC_YELLOW"hex"AC_RESET"). For bytes only "AC_YELLOW"hex"AC_RESET" is allowd (e.g. "AC_MAGENTA"0x3"AC_RESET", "AC_MAGENTA"0x56"AC_RESET", "AC_MAGENTA"0x1337AbcDEf"AC_RESET" etc.)\n");
    printf(AC_GREEN "cd                " AC_RESET "Change directory, possible inputs: "
        AC_CYAN "." AC_RESET ", "
        AC_CYAN ".." AC_RESET ", "
        AC_CYAN "<one_folder>" AC_RESET " (this is not bash command!)\n");
    printf(AC_GREEN "help" AC_RESET ", " AC_GREEN "h" AC_RESET "           " AC_RESET "Print this help\n");
    printf(AC_GREEN "other             " AC_RESET "And other normal commmands, what your " AC_YELLOW "bash" AC_RESET " can do ;-)\n");
    printf(AC_GREEN "exit"AC_RESET", "AC_GREEN"e"AC_RESET"           Terminate the program\n");
}

/*
example for string concatination:
    char input[128];
    char* x = input;
    for (; *x != 0; x++); sprintf(x, "%d", 2323);
    for (; *x != 0; x++); sprintf(x, "  %x", 2323);
    for (; *x != 0; x++); sprintf(x, "  blablablba = 432423");
    printf("%s\n", input);
*/

int main(int argc, char* const argv[]) {
    system("reset");
    getcwd(current_path, sizeof(current_path));
    memset(file_path, 0, 1024); // e.g.

    char input[INPUT_LENGTH];
    char input_copy[INPUT_LENGTH];
    char* commands[COMMANDS];
    while (1) {
        printf("%s: ", current_path);
        int idx = 0;
        int error_ret = read_input(input, commands, &idx);

        if (error_ret) {
            print_error_message(error_ret);
            continue;
        }

        get_normal_input(idx, input, input_copy, commands);

        for (int i = 0; i < idx; i++) {
            printf("command %d: %s\n", i, *(commands+i));
        }

        CommandNumber cn = get_command_number(commands[0]);

        int is_exiting_program = 0;
        switch (cn) {
            case EXIT: is_exiting_program = 1; break;
            case NEWPROGRAM: command_newprogram(); break;
            case DEFINES: command_defines(); break;
            case SETFILE: command_setfiles(commands, idx); break;
            case DRIVER: command_driver(commands, idx); break;
            case GETSECTOR: command_getsector(commands, idx); break;
            case SETSUDO: command_setsudo(commands, idx); break;
            case WRITEAT: command_writeat(commands, idx); break;
            case SETZEROSECTOR: command_setzerosector(commands, idx); break;
            case SETONESSECTOR: command_setonessector(commands, idx); break;
            case GETFILE_ZERO_ONES_SECTORS: command_getfile_zero_ones_sectors(); break;
            case PRINTARGS: command_printargs(argc, (void*)argv); break;
            case CD: command_cd(commands, idx); break;
            case HELP: command_print(); break;
            default: system(input_copy); // BASH
        }

        if (is_exiting_program) {
            break;
        }
    }

    return 0;
}
