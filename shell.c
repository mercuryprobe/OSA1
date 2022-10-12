#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cd(char* cmd) {
    
    printf("ok");
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

    char[256] buf;
    if (strcmp(tokenInput, "cd")==0) {
        // reminder: handle "cd\n" case
        
        char *getcwd(char *buf, size_t sizeof(buf));
        printf("%s\n", buf);
        printf("Changing directory...\n");
        char *getcwd(char *buf, size_t sizeof(buf));
        printf("%s\n", buf);

        // cd(tokenInput);
        chdir("..");
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}