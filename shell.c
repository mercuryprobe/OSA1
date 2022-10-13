#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void cd(char* cmd, char flag1[], char flag2[]) {
    char cwd[256];
    const char space[2] = " ";

    //get current working directory
    getcwd(cwd, sizeof(cwd));
    printf("Current directory: %s\n", cwd);

    //changing directory
    const char* directory = cmd;
    printf("Directory requested: %s", directory);
    printf("Changing directory...\n");
    int chdirResult = chdir(directory);
    
    if (chdirResult==0) {
        //directory changed successfully
        getcwd(cwd, sizeof(cwd));
        printf("Current directory: %s\n", cwd);
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
                strcpy(flag1, flagComputeText);
            } else if (flag2Taken==0) {
                strcpy(flag2, flagComputeText);
            }
            flagComputeText = strtok(NULL, space);
        }
        

        if (flag2Taken==0) {
            flag2[0]='\0';
        } 
        if (flag1Taken==0) {
            flag1[0]='\0';
        }

        if (strcmp(tokenInput, "exit\n")==0) {
            //exit
            return;
        }

        if (strcmp(tokenInput, "cd")==0) {
            // reminder: handle "cd\n" case
            tokenInput = strtok(NULL, space);
            cd(tokenInput, flag1, flag2);
        }
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}