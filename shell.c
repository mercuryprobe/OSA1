#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>

#define blue "\x1b[94m"
#define reset "\x1b[0m"

// #include "cat_.h"
// #include "date_.h"
// #include "ls_.h"
// #include "mkdir_.h"
// #include "rm_.h"

//general
// #include "flags.h"
// #include "splitStruc.h"
// #include "flagStruc.h"
// #include "tokeniser.h"

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

static volatile sig_atomic_t active = 1;
static volatile sig_atomic_t cat = 0;
static void interrupter(int x) {
    //reference: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
    printf("cat: %d\n", cat);
    active = 0;
}


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    signal(SIGINT, interrupter); //detect sys interrupt
    while (active || (cat && !active))
    {    
        if (cat && !active) {
            active = 1;
            cat = 0;
        }
        printf(blue "[rmnShell]$ " reset);
        
        char userInp[512]; //will store user input
        char inp2[512];
        fgets(userInp, sizeof(userInp), stdin);
        strcpy(inp2, userInp);

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

        splitString[0][strcspn(splitString[0], "\n")]=0;

        //get flag info
        int flag1 = -1;
        int flag2 = -1;

        int flag1Taken = 0;
        int flag2Taken = 0;
        i=0;
        int thread = 0;
        for (i; i<argLen; i++) {
            // printf("%d\n", i);
            if (strcmp(splitString[i], "&t")==0) {
                thread = 1;
            }
            if (splitString[i][0] == '-' || ((strcmp(splitString[0], "cat")==0) && (splitString[i][0] == '>'))) {
                //detect flags, if any
                // printf("Flag detected\n");
                if (flag1Taken == 0){
                    flag1 = i;
                    flag1Taken = 1;
                } else if (flag2Taken==0) {
                    flag2 = i;
                    flag2Taken = 1;
                } else {
                    printf("Error: Invalid number of flags detected.");
                    continue;
                }
            }
        }

        if ((strcmp(splitString[0], "exit")==0) || (strcmp(splitString[0], "e")==0)) {
            //exit
            puts("Exiting...");
            return;

        //internals
        } else if (strcmp(splitString[0], "cd")==0) {
            cd(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken);
        } else if (strcmp(splitString[0], "pwd")==0) {
            pwd(splitString, flag1, flag2); 
        } else if (strcmp(splitString[0], "echo")==0) {
            echo(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);

        //externals
        } else if (strcmp(splitString[0], "rm")==0) {
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024];
                getcwd(curLoc, sizeof(curLoc));
                strcat(curLoc, "/rm_.out");
                
                execl(curLoc, inp2, NULL);
                
            } else if(pid>0) {
                wait(NULL);
            } else {
                puts("Critical Error: fork failure.");
            }
            // rm(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);

        } else if (strcmp(splitString[0], "mkdir")==0) {
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024];
                getcwd(curLoc, sizeof(curLoc));
                strcat(curLoc, "/mkdir_.out");
                
                execl(curLoc, inp2, NULL);
                
            } else if(pid>0) {
                wait(NULL);
            } else {
                puts("Critical Error: fork failure.");
            }
            // mkdir_(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);

        } else if (strcmp(splitString[0], "date")==0) {
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024];
                getcwd(curLoc, sizeof(curLoc));
                strcat(curLoc, "/date_.out");
                
                execl(curLoc, inp2, NULL);
                
            } else if(pid>0) {
                wait(NULL);
            } else {
                puts("Critical Error: fork failure.");
            }
            // date_(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 

        } else if (strcmp(splitString[0], "cat")==0) {
                        
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024];
                getcwd(curLoc, sizeof(curLoc));
                strcat(curLoc, "/cat_.out");

                cat = 1;
                execl(curLoc, inp2, NULL);
                
            } else if(pid>0) {
                wait(NULL);
            } else {
                puts("Critical Error: fork failure.");
            }
            // cat(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 
            
        } else if (strcmp(splitString[0], "ls")==0) {
            pid_t pid = fork();
            if (pid==0) {
                char curLoc[1024];
                getcwd(curLoc, sizeof(curLoc));
                strcat(curLoc, "/ls_.out");
                
                execl(curLoc, inp2, NULL);
                
            } else if(pid>0) {
                wait(NULL);
            } else {
                puts("Critical Error: fork failure.");
            }
            // ls(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 

        } else {
            puts("Error: command not found.");
        }
    }
}


int main() {
    shell();
    return 0;
}