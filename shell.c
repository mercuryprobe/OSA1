#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void cd(char cmd[512][512], char flag1[], char flag2[], int posn) {
    //Change directory
    char cwd[256];
    const char space[2] = " ";
    
    //preparing directory input
    cmd[posn][strcspn(cmd[0], "\n")] = 0; //removes newline
    const char* directory = cmd[posn]; //chdir() needs a const input

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

        char splitString[512][512];
        int i = 0;
        while (tokenInput!=NULL) {
            strcpy(splitString[i], tokenInput);
            tokenInput = strtok(NULL, space);
            i+=1;
        }
        int argLen = i;

        while (i<512) {
            splitString[i][0] = 0;
            i+=1;
        }

        
        char flag1[16];
        char flag2[16];

        int flag1Taken = 0;
        int flag2Taken = 0;
        for (i=0; i++; i<argLen)
            if (splitString[i][0] == '-') {
                //detect flags, if any
                printf("Flag detected\n");
                if (flag1Taken == 0){
                    strcpy(flag1, splitString[i]);
                } else if (flag2Taken==0) {
                    strcpy(flag2, splitString[i]);
                } else {
                    printf("Error: Invalid number of flags detected.");
                    continue;
                }
            }
        

        if (flag2Taken==0) {
            flag2[0]='\0';
        } 
        if (flag1Taken==0) {
            flag1[0]='\0';
        }
        splitString[0][strcspn(splitString[0], "\n")]=0;
        if ((strcmp(splitString[0], "exit")==0) || (strcmp(splitString[0], "e")==0)) {
            //exit
            puts("Exiting...");
            return;
        } else if (strcmp(splitString[0], "cd")==0) {
            // reminder: handle "cd\n" case
            cd(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken);
        } else {
            puts("Error: command not found.");
        }
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}