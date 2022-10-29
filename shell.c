#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "cat_.h"
#include "date_.h"
#include "ls_.h"
#include "mkdir_.h"
#include "rm_.h"

//general
#include "flags.h"
#include "splitStruc.h"
#include "flagStruc.h"
#include "tokeniser.h"

void cd(char cmd[512][512], int flag1, int flag2, int posn) {
    //Change directory
    char originalCwd[256];
    getcwd(originalCwd, sizeof(originalCwd));
    char cwd[256];

    //flag handling
    int physical = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='L' || cmd[flag1][1]=='l') {
            // default behaviour: -L (edge case: conflicting flag entry)
        } else if (cmd[flag1][1]=='P' || cmd[flag1][1]=='p') {
            if (flag2==-1 || (flag2!=-1 && (cmd[flag2][1]=='P' || cmd[flag2][1]=='p'))) {
                //no flag2 or flag condition -P -P
                physical = 1;
            } else if ((flag2!=-1 && (cmd[flag2][1]!='L' && cmd[flag2][1]!='l'))) {
                printf("Error: Invalid flag.\n");
                return;
            }
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    
    
    
    //preparing directory input
    int nonBlankDir = (strcmp(cmd[posn], "") != 0); //edge case: blank input directory
    if (nonBlankDir) {
        cmd[posn][strcspn(cmd[posn], "\n")] = 0; //removes newline        
    }
    const char* directory = cmd[posn]; //chdir() needs a const input
    char home[512] = "/home/";
    strcat(home, getenv("USER"));
    const char* userHome = home;

    //directory change
    printf("Directory requested: <%s>\n", directory);
    printf("Changing directory...\n");
    int chdirResult;
    if (nonBlankDir) {
        chdirResult = chdir(directory);
    } else {
        chdirResult = chdir(userHome);
    }
    
    //if -P
    int pChdirresult = 0;
    if (physical==1){
        //getcwd retrieves absolute path of cwd
        getcwd(cwd, sizeof(cwd));
        pChdirresult = chdir(cwd);
    }
        

    if (chdirResult==0 && pChdirresult==0) {
        //directory changed successfully
    } else {
        //directory change failed (edge case: invalid directory entered)

        if (chdirResult==0 && pChdirresult!=0){
            chdir(originalCwd);
        }
        perror("Error");
        // printf("Error %d: Invalid directory.\n", chdirResult);
    }
}

void pwd(char cmd[512][512], int flag1, int flag2) {
    //present working directory

    //flag handling
    int logical = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='P' || cmd[flag1][1]=='p') {
            // default behaviour: -P (edge case: conflicting flag entry)
        } else if (cmd[flag1][1]=='L' || cmd[flag1][1]=='l') {
            if (flag2==-1 || (flag2!=-1 && (cmd[flag2][1]=='L' || cmd[flag2][1]=='l'))) {
                //no flag2 or flag condition -P -P
                logical = 1;
            } else if ((flag2!=-1 && (cmd[flag2][1]!='P' && cmd[flag2][1]!='p'))) {
                printf("Error: Invalid flag.\n");
                return;
            }
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    
    //generate cwd
    char cwd[256];
    if (logical==0){
        getcwd(cwd, sizeof(cwd)); //gets physical/absolute cwd
    } else {
        // strcpy(cwd, getenv("PWD")); 
        printf("%s\n", getenv("PWD")); //gets logical pwd
        // readlink(getcwd(NULL, 0), cwd, sizeof(cwd));
    }
    printf(cwd);
    printf("\n");
}

void echo(char cmd[512][512], int flag1, int flag2, int posn, int last) {
    //echoes input
    int newline = 1;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='H' || cmd[flag1][1]=='h') {
            //edge case: multiple flag entry
            printf("DESCRIPTION\n\techo displays a line of text.\nFLAGS\n\t-N: ignores ending newline\n\t--help: prints this section\n");
        } else if (cmd[flag1][1]=='N' || cmd[flag1][1]=='n') {
            if (flag2==-1 || (flag2!=-1 && (cmd[flag2][1]=='N' || cmd[flag2][1]=='n'))) {
                //no flag2 or flag condition -N -N
                newline = 0;
            } else if ((flag2!=-1 && (cmd[flag2][1]!='H' && cmd[flag2][1]!='h'))) {
                printf("Error: Invalid flag.\n");
                return;
            }
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    
    if (newline==0) {
        cmd[last-1][strcspn(cmd[last-1], "\n")]=0; //edge case: input is <echo > will give new line, but <echo -n> and <echo> won't
    }
    for (int i=posn; i<last; i++) {
        if (i==(last-1)) {
            printf(cmd[i]);
        } else {
            printf("%s ", cmd[i]);
        }
    }
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("[rmnShell]$ ");
        
        char userInp[512]; //will store user input
        fgets(userInp, sizeof(userInp), stdin);
        
        //tokenise input
        struct splitStruc tokens = tokenise(userInp);

        //get flag info
        struct flagStruc floogs = flagger(tokens.splitString, tokens.argLen);
        int flag1 = floogs.flag1;
        int flag2 = floogs.flag2;
        int flag1Taken = floogs.flag1Taken;
        int flag2Taken = floogs.flag2Taken;
        int thread = floogs.thread; //0 if no, 1 if yes

        if ((strcmp(tokens.splitString[0], "exit")==0) || (strcmp(tokens.splitString[0], "e")==0)) {
            //exit
            puts("Exiting...");
            return;

        //internals
        } else if (strcmp(tokens.splitString[0], "cd")==0) {
            cd(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken);
        } else if (strcmp(tokens.splitString[0], "pwd")==0) {
            pwd(tokens.splitString, flag1, flag2); 
        } else if (strcmp(tokens.splitString[0], "echo")==0) {
            echo(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen);

        //internals
        } else if (strcmp(tokens.splitString[0], "rm")==0) {
            rm(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen);
        } else if (strcmp(tokens.splitString[0], "mkdir")==0) {
            mkdir_(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen);
        } else if (strcmp(tokens.splitString[0], "date")==0) {
            date_(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen); 
        } else if (strcmp(tokens.splitString[0], "cat")==0) {
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024] = getcwd(NULL, 0);
                strcat(curLoc, "/cat_.out");
                execl(curLoc, curLoc, userInp, NULL);
            } else if(pid>0) {
                wait();
            } else {
                puts("Critical Error: fork failure.");
            }
            // cat(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen); 
        } else if (strcmp(tokens.splitString[0], "ls")==0) {
            ls(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen); 
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