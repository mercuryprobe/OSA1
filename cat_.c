#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "flags.h"
#include "splitStruc.h"
#include "flagStruc.h"
#include "tokeniser.h"

static volatile sig_atomic_t active = 1;
static void interrupter(int x) {
    active = 0;
}
void cat(char cmd[512][512], int flag1, int flag2, int posn, int last, int t) {
    //cat
    //flags: -n (line numbering) and > (newfile)
    //press Ctrl+C to exit newfile text input
    printf(cmd[posn]);
    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    
    //flag check
    int n = 0;
    int c = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='n' || cmd[flag1][1]=='N') {
            n = 1;
        } else if (cmd[flag1][0]=='>') {
            c = 1;
        } else {
            printf("Invalid flag entered.\n");
            return;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='n' || cmd[flag2][1]=='N') {
            n = 1;
        } else if (cmd[flag2][0]=='>') {
            c = 1;
        } else {
            printf("Invalid flag entered.\n");
            return;
        }
    }

    FILE *file;
    char text[1024];
    
    if (c==0) {
        //if > isn't entered
        file = fopen(cmd[posn], "r");

        if (file==NULL) {
            //edge case: input is empty
            printf("Error: File not found.\n");
            return;
        } 

        if (n == 0) {
            while (fgets(text, 1024, file)!=NULL) {
            printf(text);
            }
        } else {
            int line = 1;
            while (fgets(text, 1024, file)!=NULL) {
            printf("%d %s", line, text);
            line +=1;
            }
        }
    } else {
        if (last!=2){
            // printf("posn: %d\nlast: %d\n");
            file = fopen(cmd[posn], "w");
            
            signal(SIGINT, interrupter); //detect sys interrupt
            while(active) {
                fgets(text, 1024, stdin);
                if (active ==1) {
                        //to prevent unintentional ending newline
                        fprintf(file, text);
                    }
            }
            active = 1;
        } else {
            //edge case: no filename input after cat >
            printf("Error: No filename entered.\n");
            return;
        } 
    }
    
    fclose(file);
}

int main(int argc, char *argv[]) {
    //tokenise input
    struct splitStruc tokens;
    if (argc!=1) {
        puts("rip");
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
    cat(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen, floogs.thread);

    return 0;
}

