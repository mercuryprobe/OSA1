#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cd(char* cmd) {
    char buf[256];    
    getcwd(buf, sizeof(buf));
    printf("%s\n", buf);
    printf("Changing directory...\n");
    chdir(cmd);
    getcwd(buf, sizeof(buf));
    printf("%s\n", buf);

    printf("ok");
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("rmnShell>");
        
        char userInp[512]; //will store user input
        fgets(userInp, sizeof(userInp), stdin);

        char *tokenInput = strtok(userInp, " ");
        char *flagComputeText = strtok(userInp, " ");
        
        char flag1[64];
        char flag2[64];

        int flag1Taken = 0;
        while (flagComputeText != NULL)
        {
            if (flagComputeText[0]=='-'){
                if (flag1Taken == 0){
                    strcpy(flag1, flagComputeText);
                } else {
                    strcpy(flag2, flagComputeText);
                }
            }
            flagComputeText = strtok(NULL, " ");
        }
        // puts(flag1);
        // puts(flag2);
        // puts(flagComputeText);

        if (strcmp(tokenInput, "exit\n")==0) {
            //exit
            return;
        }

        if (strcmp(tokenInput, "cd")==0) {
            //
            // reminder: handle "cd\n" case
            
            // cd(tokenInput);
            
        }
    }
}


int main() {
    puts("Welcome to Aryaman's shell!");
    shell();
    return 0;
}