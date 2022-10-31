#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#define _XOPEN_SOURCE 500 
#define __USE_XOPEN_EXTENDED 500
#include <ftw.h>
#include <limits.h>
#include <sys/stat.h> 

#include "flags.h"
#include "splitStruc.h"
#include "flagStruc.h"
#include "tokeniser.h"

static int a = 0;
// int isSym(const char file[]) {
//     //removed because it didnt work        

//     //returns 0 if file is symlink, 1 otherwise
//     //reference[1]: https://man7.org/linux/man-pages/man3/lstat.3p.html
//     //reference[2]: https://www.linuxquestions.org/questions/programming-9/checking-whether-a-file-is-symbolic-link-or-not-274533/
    
//     struct stat fileStat;
//     int res = lstat(file, &fileStat); 
    
//     if (res==0) {
//         if (S_ISLNK(fileStat.st_mode) == 1) {
//             return 0;
//         }
//     }
//     return 1;
// }
int lister(const char *path, const struct stat *s, int flag, struct FTW *ftw) {
    // lister function, run on every node (directory) during file tree walk by nftw
    // printf("%s\n", path);
    DIR *directory;
    directory = opendir(path);


    if (directory!=NULL) {    
        struct dirent *dirStruc = readdir(directory);

        if (dirStruc!=NULL) {
            for (int i =0; i<strlen(path); i++) {
                //edge case: hidden subdirectories present
                if (path[i]=='.') {
                    return 0;
                }
            }
            printf("%s:\n   ", path);
        }
        
        while (dirStruc!=NULL) {

            if ((a==1 && ((dirStruc->d_name)[0]=='.')) || ((dirStruc->d_name)[0]!='.')) {
                printf("%s  ", dirStruc->d_name);
            }
            dirStruc = readdir(directory);
        }
        printf("\n\n");
    }
    return 0;
}
void ls(char cmd[512][512], int flag1, int flag2, int posn, int last, int t) {
    if (last!=1) {
        cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    }

    //flag check
    int r = 0;
    if (flag1!=-1) {
        if (cmd[flag1][1]=='a') {
            a = 1;
        } else if (cmd[flag1][1]=='r' || cmd[flag1][1]=='R') {
            r = 1;
        } else {
            printf("Invalid flag entered.\n");
            return;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='a') {
            a = 1;
        } else if (cmd[flag2][1]=='r' || cmd[flag2][1]=='R') {
            r = 1;
        } else {
            printf("Invalid flag entered.\n");
            a = 0;
            return;
        }
    }
    
    if (r==0) {
        //non recursive flag
        DIR *directory;

        if (last==2 && (strcmp(cmd[posn], "")==0)) {
            //edge case: input <ls >
            last-=1;
        }

        if (last>=2) {
            //input is <ls path> or edge case: <ls path ...(ignored)..>
            directory = opendir(cmd[posn]);
        } else {
            //input is <ls>
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            directory = opendir(cwd);
        }

        struct dirent *dirStruc = readdir(directory);
        while (dirStruc!=NULL) {
            if ((a==1 && ((dirStruc->d_name)[0]=='.')) || ((dirStruc->d_name)[0]!='.')) {
                printf("%s  ", dirStruc->d_name);
            }
            dirStruc = readdir(directory);
        }
        printf("\n");

    } else {
        int recurResult = 0;
        if (last>=3) {
            //input is <ls -r path> or edge case: <ls -r path ...(ignored)..>
            recurResult = nftw(cmd[posn], lister, FOPEN_MAX, FTW_DEPTH);
        } else {
            //input is <ls -r>
            //DOES NOT WORK IF SYMLINKS PRESENT IN DIRECTORY. Handled by perror.
            char cwd[256];
            getcwd(cwd, sizeof(cwd));
            recurResult = nftw(cwd, lister, FOPEN_MAX, FTW_DEPTH);
        }

        if (recurResult!=0) {
            perror("Error");
        }
    }
    a = 0;
}

int main(int argc, char *argv[]) {
    //tokenise input
    struct splitStruc tokens;
    for (int i =0; i<argc; i++) {
        strcpy(tokens.splitString[i], argv[i]);
    }
    tokens.argLen = argc;
    
    tokens.splitString[0][strcspn(tokens.splitString[0], "\n")]=0;

    //flag detection
    struct flagStruc floogs = flagger(tokens.splitString, argc+1);
    int flag1 = floogs.flag1;
    int flag2 = floogs.flag2;
    int flag1Taken = floogs.flag1Taken;
    int flag2Taken = floogs.flag2Taken;

    //run function
    ls(tokens.splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, tokens.argLen, floogs.thread);

    return 0;
}