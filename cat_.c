#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static volatile sig_atomic_t active = 1;
static void interrupter(int x) {
    //reference: https://stackoverflow.com/questions/4217037/catch-ctrl-c-in-c
    active = 0;
}
void cat(char cmd[512][512], int flag1, int flag2, int posn, int last) {
    //cat
    //flags: -n (line numbering) and > (newfile)
    //press Ctrl+C to exit newfile text input
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