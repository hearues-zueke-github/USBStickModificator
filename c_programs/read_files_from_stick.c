#include "read_files_from_stick.h"

char current_path[1024];
char file_path[1024];

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

void print_error_message(int error_ret) {
    switch (error_ret) {
        case 1:
            printf(AC_RED_BOLD "Error:" AC_RESET " max char length: %d\n", INPUT_LENGTH);
        case 2:
            printf(AC_RED_BOLD "Error:" AC_RESET " not more than %d commands!\n", COMMANDS);
    }

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
        if (!(((*c >= '0') && (*c <= '9')) ||
              ((*c >= 'A') && (*c <= 'F')) ||
              ((*c >= 'a') && (*c <= 'f')))) {
            return 0;
        }
    }
    return 1;
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

void command_setsudo(char** commands, int idx) {
    // TODO: do this check everywhere, before open a file!
    if (access(file_path, F_OK)) {
        printf(AC_RED_BOLD "Error:" AC_RESET " file " AC_GREEN "%s" AC_RESET " does not exist!\n", file_path);
        return;
    }
    char username[1024];
    getlogin_r(username, sizeof(username));
    char bash_command[1024];
    sprintf(bash_command, "sudo chown %s:%s %s", username, username, file_path);
    system(bash_command);
    printf("File " AC_GREEN "%s" AC_RESET " successfull given rights to user " AC_GREEN "%s" AC_RESET "\n", file_path, username);
}

void command_writeat(char** commands, int idx) {
    if (idx < 3) {
        printf(AC_RED_BOLD "Error:" AC_RESET " 2 arguments needed! (first: pos, second: byte value)\n"); 
        return; 
    }

    int pos = 0;
    uint8_t byte = 0;

    if (is_number_decimal(commands[1])) {
        pos = strtol(commands[1], NULL, 10);
    } else if (is_number_hex(commands[1])) {
        pos = strtol(commands[1], NULL, 16);
    } else {
        printf(AC_RED_BOLD "Error:" AC_RESET " first argument is not a decimal number!\n");  
        return;
    }

    if (is_number_decimal(commands[2])) {
        byte = strtol(commands[2], NULL, 10) % 0x100;
    } else if (is_number_hex(commands[1])) {
        byte = strtol(commands[2], NULL, 16) % 0x100;
    } else {
        printf(AC_RED_BOLD "Error:" AC_RESET " second argument is not a decimal number!\n");
        return;
    }
    FILE* f_drive = fopen(file_path, "wb");
    fseek(f_drive, pos, SEEK_SET);
    fwrite((void*)&byte, 1, 1, f_drive);
    fclose(f_drive);
    printf("Successfully written at pos " AC_GREEN "0x%X" AC_RESET " byte " AC_GREEN "0x%X" AC_RESET " in file " AC_GREEN "%s" AC_RESET "\n", pos, byte, file_path);
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
    printf(AC_GREEN "other             " AC_RESET "And other normal commmands, what your " AC_YELLOW "bash" AC_RESET " can do ;-)\n");
    printf(AC_GREEN "help" AC_RESET ", " AC_GREEN "h" AC_RESET "           " AC_RESET "Print this help\n");
    
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

        CommandNumber cn = get_command_number(commands[0]);
        int is_exiting_program = 0;
        switch (cn) {
            case EXIT:
                is_exiting_program = 1;
                break;
            case NEWPROGRAM:
                command_newprogram();
                break;
            case DEFINES:
                command_defines();
                break;
            case SETFILE:
                command_setfiles(commands, idx);
                break;
            case DRIVER:
                command_driver(commands, idx);
                break;
            case GETSECTOR:
                command_getsector(commands, idx);
                break;
            case SETSUDO:
                command_setsudo(commands, idx);
                break;
            case WRITEAT:
                command_writeat(commands, idx);
                break;
            case PRINTARGS:
                command_printargs(argc, (void*)argv);
                break;
            case CD:
                command_cd(commands, idx);
                break;
            case HELP:
                command_print();
                break;
            default: // BASH
                system(input_copy);
        }

        if (is_exiting_program) {
            break;
        }

    }

    return 0;
}
