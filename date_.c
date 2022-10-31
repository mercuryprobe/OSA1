#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#define _XOPEN_SOURCE 1 
#include <time.h>

#include "flags.h"
#include "splitStruc.h"
#include "flagStruc.h"
#include "tokeniser.h"

void date_(char cmd[512][512], int flag1, int flag2, int posn, int last, int t) {
    //date
    //flags: -u (UTC-Time), -R (RFCs 5322 date)

    //flag detection
    int u = 0;
    int r = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='u' || cmd[flag1][1]=='U') {
            u = 1;
        }
        if ((cmd[flag1][1]=='r' || cmd[flag1][1]=='R')) {
            r = 1;
        } else {
            puts("Invalid flag entered.");
            return;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='u' || cmd[flag2][1]=='U') {
            u = 1;
        }
        if ((cmd[flag2][1]=='r' || cmd[flag2][1]=='R')) {
            r = 1;
        } else {
            puts("Invalid flag entered.");
            return;
        }
    }

    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    
    int flag1Taken;
    int flag2Taken;
    if (flag1==-1) {flag1Taken = 0;} else {flag1Taken = 1;}
    if (flag2==-1) {flag2Taken = 0;} else {flag2Taken = 1;}
    if (last>1+flag1Taken+flag2Taken) {puts("Ignoring invalid argument...");} //edge case: arguments given to date beyond flags
    time_t ti;
    time(&ti);
    
    if (u==0 && r==0) {
        //default output
        printf(ctime(&ti));
    } else if (u==1){
        //GMT time
        struct tm *gmtTm = gmtime(&ti);
        time_t gmt = mktime(gmtTm);
        printf(ctime(&gmt));

    } else {
        struct tm *inpTime = localtime(&ti);
        const char format[] = "%a, %d %b %Y %H:%M:%S %z";
        char rfcDate[64];
        strftime(rfcDate, sizeof(rfcDate), format, inpTime);
        puts(rfcDate);
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
    date_(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen, floogs.thread);

    return 0;
}