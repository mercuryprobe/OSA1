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
        
        // char userInp[512]; //will store user input
        // fgets(userInp, sizeof(userInp), stdin);
        
        char str[80] = "This is a test";
        const char s[2] = " ";
        char *token;
        
        /* get the first token */
        token = strtok(str, s);
        
        /* walk through other tokens */
        while( token != NULL ) {
        printf( " %s\n", token );
        
        token = strtok(NULL, s);}

        return
        // char userInp[] = "This is a test for this thing";
        // const char space[] = " ";
        // char *tokenInput;
        // char *flagComputeText;
        // tokenInput = strtok(userInp, space);
        // flagComputeText = strtok(userInp, space);
        
        // char flag1[64];
        // char flag2[64];

        // int flag1Taken = 0;
        // while (flagComputeText != NULL)
        // {
        //     puts(flagComputeText);
        //     if (flagComputeText[0]=='-'){
        //         printf("hmmm\n");
        //         if (flag1Taken == 0){
        //             strcpy(flag1, flagComputeText);
        //         } else {
        //             strcpy(flag2, flagComputeText);
        //         }
        //     }
        //     flagComputeText = strtok(NULL, space);
        //     if (flagComputeText==NULL) {
        //         puts("End!");
        //     }
        // }
        // printf("%s bruh\n",flag1);
        return;
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