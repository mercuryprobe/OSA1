#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cd(char* cmd, char flag1[], char flag2[]) {
    char cwd[256];
    const char space[2] = " ";

    //get current working directory
    getcwd(cwd, sizeof(cwd));
    printf("Current directory: %s\n", cwd);

    //skip flags before inputting token into chdir
    while (cmd[0]=='-') {
        cmd = strtok(NULL, space);
    }
    const char* directory = cmd;

    //token ready
    printf("Changing directory...\n");
    int chdirResult = chdir(directory);
    
    if (chdirResult==0) {
        //directory changed successfully
        getcwd(cwd, sizeof(cwd));
        printf("Current directory: %s\n", cwd);
    } else {
        //directory change failed
        printf("Error %d: Invalid directory.\n", chdirResult);
    }
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("rmnShell>");
        
        char userInp[512]; //will store user input
        fgets(userInp, sizeof(userInp), stdin);
        
        char userInpCopy[512];
        strcpy(userInpCopy, userInp);
        
        //tokenise input
        const char space[2] = " ";
        char* tokenInput;
        char* flagComputeText;
        tokenInput = strtok(userInp, space);
        flagComputeText = strtok(userInpCopy, space);
        
        char flag1[16];
        char flag2[16];

        int flag1Taken = 0;
        int flag2Taken = 0;
        while (flagComputeText != NULL)
        {
            //detect flags, if any
            if (flagComputeText[0]=='-'){
                printf("Flag detected\n");
                if (flag1Taken == 0){
                    strcpy(flag1, flagComputeText);
                } else if (flag2Taken==0) {
                    strcpy(flag2, flagComputeText);
                }
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
            cd(tokenInput, flag1, flag2);
        }
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}