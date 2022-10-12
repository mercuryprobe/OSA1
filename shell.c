#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void cd(char* cmd) {
    puts("Hey");
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    puts("Enter command: ");
    
    char userInp[512]; //will store user input
    fgets(userInp, sizeof(userInp), stdin);

    char *tokenInput = strtok(userInp, "");

    if (strcmp(tokenInput, "cd")) {
        puts("Changing directory...\n");
        cd(tokenInput);
    }
}


int main() {
    puts("Welcome to Aryaman's shell!\n");
    shell();
    return 0;
}