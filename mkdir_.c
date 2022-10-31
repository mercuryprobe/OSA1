#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include "flags.h"
#include "splitStruc.h"
#include "flagStruc.h"
#include "tokeniser.h"

void mkdir_(char cmd[512][512], int flag1, int flag2, int posn, int last, int t) {
    //makes dir, supports multiple input
    //flags: -m (modes), -v (verbose)
    last -= 1;

    //flag detection
    int verbose = 0;
    int modeFlag = 0;
    char mode[4];
    if (flag1!=-1) {
        if (cmd[flag1][1] == 'v' || cmd[flag1][1] == 'V') {
            verbose = 1;
        } else if (cmd[flag1][1] == 'M' || cmd[flag1][1] == 'm') {
            if (flag2==-1) {
                if (last!=flag1) {
                    //edge case: input mode is a string longer than 3 chars
                    mode[0] = cmd[flag1+1][0];
                    mode[1] = cmd[flag1+1][1];
                    mode[2] = cmd[flag1+1][2];
                    modeFlag = 1;

                    posn+=1;
                    if (posn>last) {
                        printf("Error: No directory entered.\n");
                        return;
                    }
                } else {
                    //edge case: mode not entered
                    printf("Error: Mode not entered.\n");
                    return;
                }
            } else {
                //edge case: mode may/not be entered but not immediately after -m flag
                printf("Error: Improper mode input.\n");
                return;
            }
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1] == 'v' || cmd[flag2][1] == 'V') {
            verbose = 1;
        } else if (cmd[flag2][1] == 'M' || cmd[flag2][1] == 'm') {
            if (last!=flag2) {
                    //edge case: input mode is a string longer than 3 chars
                    mode[0] = cmd[flag2+1][0];
                    mode[1] = cmd[flag2+1][1];
                    mode[2] = cmd[flag2+1][2];
                    modeFlag = 1;

                    posn+=1;
                    if (posn>last) {
                        //edge case: dirname not entered
                        printf("Error: No directory entered.\n");
                        return;
                    }
                } else {
                    //edge case: mode not entered
                    printf("Error: Mode not entered.\n");
                    return;
                }
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }

    int multiple = 1;    
    if (posn==last) {
        multiple = 0;
    }

    cmd[last][strcspn(cmd[last], "\n")]=0;

    int mkdirResult = 0;
    int i = posn;
    for (i; i<(last+1); i++) {
        //makes directory, A: All permissions (rwx) is default
        char modeChar;
        if (modeFlag == 0 || (strcmp(mode, "rwx")==0) || (strcmp(mode, "RWX")==0)) {
            mkdirResult = mkdir(cmd[i], S_IRWXU);
            modeChar='A';
        } else if (mode[0]=='r' || mode[0]=='R') {
            mkdirResult = mkdir(cmd[i], S_IRUSR);
            modeChar='R';
        } else if (mode[0]=='w' || mode[0]=='W') {
            mkdirResult = mkdir(cmd[i], S_IWUSR);
            modeChar='W';
        } else if (mode[0]=='x' || mode[0]=='X') {
            mkdirResult = mkdir(cmd[i], S_IXUSR);
            modeChar='X';
        } else {
            printf("Error: Invalid mode entered.\n");
            return;
        }
        if ((mkdirResult!=0) && (multiple==1)) {
            //edge case: error making one of multiple dirnames
            printf("Error while making directory: %s\n", cmd[i]);
            break;
        }
        if (verbose==1 && mkdirResult==0) {
            printf("Directory created: %s [mode: %c]\n", cmd[i], modeChar);
        }
        
    }

    if (mkdirResult==0) {
        if (posn>last) {
            //edge case: no dirname entered
            printf("Error: Missing operand\n");
        }
        return;
    } else {
        perror("Error");
        if (multiple==1 && (i!=last)) {
            //if multiple dirs have been mentioned
            printf("Skipping...\n");
            mkdir_(cmd, flag1, flag2, i+1, last+1, t);
        }
    }
}

int main(int argc, char *argv[]) {
    //tokenise input
    struct splitStruc tokens;
    if (argc!=1) {
        for (int i =0; i<argc; i++) {
            strcpy(tokens.splitString[i], argv[i]);
        }
        tokens.argLen = argc;
        
    } else {
        struct splitStruc tokens = tokenise(argv[0]);
    }
    
    tokens.splitString[0][strcspn(tokens.splitString[0], "\n")]=0;

    //flag detection
    struct flagStruc floogs = flagger(tokens.splitString, tokens.argLen);
    int flag1 = floogs.flag1;
    int flag2 = floogs.flag2;
    int flag1Taken = floogs.flag1Taken;
    int flag2Taken = floogs.flag2Taken;

    //run function
    mkdir_(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen, floogs.thread);

    return 0;
}