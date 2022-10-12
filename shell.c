#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    put("Welcome to Aryaman's shell!\n");
    shell();
    return 0;
}

void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    put("Enter command: ");
    
    char userInp[512]; //will store user input
    fgets(userInp, sizeof(userInp), stdin);

    char *tokenInput = strtok(userInp, "");

    if (strcmp(tokenInput, "cd")) {
        put("Changing directory...\n");
        cd(tokenInput);
    }
}

void cd(char* cmd) {
    put("Hey");
    return(0);
}