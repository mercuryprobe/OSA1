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

void date_(char cmd[512][512], int flag1, int flag2, int posn, int last) {
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
            d = 1;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='u' || cmd[flag2][1]=='U') {
            u = 1;
        }
        if ((cmd[flag2][1]=='r' || cmd[flag2][1]=='R')) {
            r = 1;
        }
    }

    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;

    time_t t;
    time(&t);
    
    if (u==0 && r==0) {
        //default output
        printf(ctime(&t));
    } else if (u==1){
        //GMT time
        struct tm *gmtTm = gmtime(&t);
        time_t gmt = mktime(gmtTm);
        printf(ctime(&gmt));

    } else {
        struct tm *inpTime = localtime(&t);
        const char format[] = "%a, %d %b %Y %H:%M:%S %z";
        char rfcDate[64];
        strftime(rfcDate, sizeof(rfcDate), format, inpTime);
        puts(rfcDate);
    }
}

int main(int argc, char *argv[]) {
    //tokenise input
    struct splitStruc tokens = tokenise(argv[0]);
    tokens.splitString[0][strcspn(tokens.splitString[0], "\n")]=0;

    //flag detection
    struct flagStruc floogs = flagger(tokens.splitString, argc+1);
    int flag1 = floogs.flag1;
    int flag2 = floogs.flag2;
    int flag1Taken = floogs.flag1Taken;
    int flag2Taken = floogs.flag2Taken;

    //run function
    date_(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen);

    return 0;
}