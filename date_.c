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
    //flags: -u (UTC-Time), -R (RTF time)

    //flag detection
    int u = 0;
    int d = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='u' || cmd[flag1][1]=='U') {
            u = 1;
        }
        if ((cmd[flag1][1]=='d' || cmd[flag1][1]=='D')) {
            d = 1;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='u' || cmd[flag2][1]=='U') {
            u = 1;
        }
        if ((cmd[flag2][1]=='d' || cmd[flag2][1]=='D')) {
            d = 1;
        }
    }

    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    if (d==0) {
        time_t t;
        time(&t);
        
        if (u==0) {
            //default output
            printf(ctime(&t));
        } else {
            //GMT time
            struct tm *gmtTm = gmtime(&t);
            time_t gmt = mktime(gmtTm);
            printf(ctime(&gmt));
        }
    } else {
        if (last==posn+3) {
            struct tm *inpTime;
            const char format[] = "%d %m %y";
            char input[512] = "";
            strcat(input, cmd[posn]);
            strcat(input, " ");
            strcat(input, cmd[posn+1]);
            strcat(input, " ");
            strcat(input, cmd[posn+2]);

            // printf("%s\n", input);
            // strptime(input, format, inpTime);
            time_t time = mktime(inpTime);
            printf(ctime(&time));
        }
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