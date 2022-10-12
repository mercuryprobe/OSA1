#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cd(char* cmd) {
    int retVal = chdir("..");
    printf("%d", retVal);
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    printf("Enter command: ");
    
    char userInp[512]; //will store user input
    fgets(userInp, sizeof(userInp), stdin);

    char *tokenInput = strtok(userInp, " ");
    // printf("%d", strcmp(tokenInput, "cd\n"));
    // printf("%s",tokenInput);

    if (strcmp(tokenInput, "cd")==0) {
        // reminder: handle "cd\n" case
        printf("Changing directory...\n");
        cd(tokenInput);
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}