#include "read_files_from_stick.h"

char current_path[1024];

int read_input(char* input, char** commands, int* idx) {
    memset(input, 0, INPUT_LENGTH);
    char* c = input;
    int length = 0;
    *idx = 1;
    int is_last_space = 0;
    // Get the whole input from the console
    commands[0] = c;
    while ((*c = getchar()) != '\n') {
        length++;
        if (length >= INPUT_LENGTH) {
            while ((*c = getchar()) != '\n');
            return 1;
        }
        if (*c == ' ') {
            *c = 0;
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
        c++;
    }

    *c = 0;

    return 0;
}

void get_normal_input(int idx, char* input, char* input_copy, char** commands) {
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
        if (!(((*c >= '0') && (*c <= '9')) ||
              ((*c >= 'A') && (*c <= 'F')) ||
              ((*c >= 'a') && (*c <= 'f')))) {
            return 0;
        }
    }
    return 1;
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
    char file_path[1024] = {0}; // = "/dev/sdb"; // the usb stick

    char input[INPUT_LENGTH];
    char input_copy[INPUT_LENGTH];
    char* commands[COMMANDS];
    while (1) {
        printf("%s: ", current_path);

        int idx = 0;
        int error_ret = read_input(input, commands, &idx);

        switch (error_ret) {
            case 1:
                printf(AC_RED_BOLD "Error:" AC_RESET " max char length: %d\n", INPUT_LENGTH);
                continue;
            case 2:
                printf(AC_RED_BOLD "Error:" AC_RESET " not more than %d commands!\n", COMMANDS);
                continue;
        }

        get_normal_input(idx, input, input_copy, commands);

        // TODO: need to move this to an other function!
        if (!strcmp(commands[0], "exit") ||
            !strcmp(commands[0], "e")) {
            printf("Exiting program...\n");
            break;
        } else if (!strcmp(commands[0], "n")) {
            system("make");
            char program_name[] = "./read_files_from_stick";
            execl(program_name, program_name, NULL);
        }
        else if (!strcmp(commands[0], "defines")) {
            printf("INPUT_LENGTH: %d\n", INPUT_LENGTH);
            printf("COMMANDS    : %d\n", COMMANDS);
        }
        else if (!strcmp(commands[0], "setfile")) {
            if (idx < 2) {
                printf(AC_RED_BOLD "Error:" AC_RESET " need 1 argument: " AC_GREEN "file_path" AC_RESET "\n");
                continue;
            }
            sprintf(file_path, commands[1]);
        }
        else if (!strcmp(commands[0], "driver")) {
            if (idx < 2) {
                printf(AC_RED_BOLD "Error:" AC_RESET " need 1 argument: " AC_GREEN "driver_name" AC_RESET "\n");
                continue;
            }
            sprintf(file_path, "/dev/%s", commands[1]);
        }
        else if (!strcmp(commands[0], "getsector") ||
                 !strcmp(commands[0], "gsec")) {
            int lba = 0;
            if (idx >= 2) {
                if (is_number_decimal(commands[1])) {
                    lba = strtol(commands[1], NULL, 10);
                } else if (is_number_hex(commands[1])) {
                    lba = strtol(commands[1], NULL, 16);
                } else {
                    printf(AC_RED_BOLD "Error:" AC_RESET " second argument is not a decimal number!\n");
                    continue;   
                }
            }
            FILE* f_drive = fopen(file_path, "rb");
            if (f_drive == NULL) {
                printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist\n", file_path);
                continue;
            }
            uint8_t block[SECTOR_SIZE];
            fseek(f_drive, SECTOR_SIZE*lba, SEEK_SET);
            fread((void*)block, 1, SECTOR_SIZE, f_drive);
            fclose(f_drive);
            printSector(block, SECTOR_SIZE, 16, lba);
        }
        else if (!strcmp(commands[0], "printargs")) {
            printf("argc: %d\n", argc);
            for (int i = 0; i < argc; i++) {
                printf("argv[%d] = %s\n", i, argv[i]);
            }
        }
        else if (!strcmp(commands[0], "cd")) {
            if (idx < 2) {
                printf(AC_RED_BOLD "Error:" AC_RESET " command " AC_GREEN "cd" AC_RESET " takes one argument\n");
                continue;
            }
            if (!strcmp(commands[1], ".")) {
                continue;
            } else if (!strcmp(commands[1], "..")) {
                char* c = current_path;
                while (*c != 0) { c++; }

                while (*c != '/') { *c = 0; c--; }
                if (c != current_path) {
                    *c = 0;
                }
                chdir(current_path);

                continue;
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
                continue;
            }
        } else if (!strcmp(commands[0], "help") ||
                   !strcmp(commands[0], "h")) {
            printf(AC_MAGENTA "commands:\n" AC_RESET);
            printf(AC_GREEN "defines           " AC_RESET "Print all given defines\n");
            printf(AC_GREEN "printargs         " AC_RESET "Print the given argc and argv parameters\n");
            printf(AC_GREEN "setfile           " AC_RESET "Set the path to the file (absolute)\n");
            printf(AC_GREEN "driver            " AC_RESET "Set the path to the file (" AC_CYAN "/dev/" AC_RESET " as preffix)\n");
            printf(AC_GREEN "getsector, gsec   " AC_RESET "Prints the sector block with the given " AC_CYAN "index" AC_RESET " ("AC_YELLOW"decimal"AC_RESET" or "AC_YELLOW"hex"AC_RESET" value)\n");
            printf(AC_GREEN "cd                " AC_RESET "Change directory, possible inputs: "
                AC_CYAN "." AC_RESET ", "
                AC_CYAN ".." AC_RESET ", "
                AC_CYAN "<one_folder>" AC_RESET " (this is not bash command!)\n");
            printf(AC_GREEN "exit"AC_RESET", "AC_GREEN"e"AC_RESET"           Terminate the program\n");
            printf(AC_GREEN "other             " AC_RESET "And other normal commmands, what the your " AC_YELLOW "bash" AC_RESET " can do ;-)\n");
            printf(AC_GREEN "help" AC_RESET ", " AC_GREEN "h" AC_RESET "           " AC_RESET "Print this help\n");
        } else {
            system(input_copy);
        }
    }

    return 0;
}