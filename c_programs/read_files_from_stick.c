#include "file_system_utils.h"

#define INPUT_LENGTH 16
#define COMMANDS 3

int read_input(char* input, char** commands, int* idx) {
    memset(input, 0, INPUT_LENGTH);
    char* c = input;
    int length = 0;
    *idx = 1;
    int is_last_space = 0;
    // Get the whole input from the console
    // while ((*c = getchar()) == ' ') { c++; }
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
                return 2;
            }
            is_last_space = 0;
        }
        c++;
    }

    *c = 0;

    return 0;
}
int main(int argc, char* const argv[]) {
    // char file_path[] = "/dev/sdb"; // the usb stick
    printf("Hello World!\n");

    char input[INPUT_LENGTH];
    char* commands[COMMANDS];
    while (1) {
        printf(">: ");

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

        // printf("idx: %d\n", idx);
        // printf("input: %s\n", input);

        // printf("get input block:\n");
        // printBlock((void*)input, INPUT_LENGTH, 16);
        // for (int i = 0; i < idx; i++) {
        //     printf("command %d: %s\n", i+1, commands[i]);
        // }

        if (!strcmp(commands[0], "exit")) {
            printf("Exiting program...\n");
            break;
        }
    }

    return 0;
}
