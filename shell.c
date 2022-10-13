#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void cd(char* cmd, char flag1[], char flag2[]) {
    //Change directory
    char cwd[256];
    const char space[2] = " ";

    //preparing directory input
    cmd[strcspn(cmd, "\n")] = 0; //removes newline
    const char* directory = cmd; //chdir() needs a const input

    //directory change
    printf("Directory requested: <%s>\n", directory);
    printf("Changing directory...\n");
    int chdirResult = chdir(directory);
    
    if (chdirResult==0) {
        //directory changed successfully
        getcwd(cwd, sizeof(cwd));
    } else {
        //directory change failed
        perror("Error");
        // printf("Error %d: Invalid directory.\n", chdirResult);
    }
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("rmnShell>");
        
        char userInp[512]; //will store user input
        fgets(userInp, sizeof(userInp), stdin);
        
        //tokenise input
        const char space[2] = " ";
        char* tokenInput;
        tokenInput = strtok(userInp, space);
        
        char flag1[16];
        char flag2[16];

        int flag1Taken = 0;
        int flag2Taken = 0;
        while (tokenInput[0] == '-') {
            //detect flags, if any
            printf("Flag detected\n");

            if (flag1Taken == 0){
                strcpy(flag1, tokenInput);
            } else if (flag2Taken==0) {
                strcpy(flag2, tokenInput);
            }
            tokenInput = strtok(NULL, space);
        }
        

        if (flag2Taken==0) {
            flag2[0]='\0';
        } 
        if (flag1Taken==0) {
            flag1[0]='\0';
        }
        tokenInput[strcspn(tokenInput, "\n")]=0;
        if ((strcmp(tokenInput, "exit")==0) || (strcmp(tokenInput, "e")==0)) {
            //exit
            puts("Exiting...");
            return;
        } else if (strcmp(tokenInput, "cd")==0) {
            // reminder: handle "cd\n" case
            tokenInput = strtok(NULL, space);
            cd(tokenInput, flag1, flag2);
        } else {
            puts("command not found.");
        }
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}