#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

//rm
#define _XOPEN_SOURCE 500 
#define __USE_XOPEN_EXTENDED 500
#include <ftw.h>
#include <limits.h>
#include <sys/stat.h>  //mdkir


int remover(const char *path, const struct stat *s, int flag, struct FTW *ftw) {
    //removes file if its not a directory (used with nftw)
    //reference[1]: https://man7.org/linux/man-pages/man3/ftw.3.html
    //reference[2]: https://stackoverflow.com/questions/70695049/nftw-remove-the-directory-content-without-removing-the-top-dir-itself
    //reference[3]: https://stackoverflow.com/questions/1149764/delete-folder-and-all-files-subdirectories
    //reference[4]: https://stackoverflow.com/questions/782338/warning-with-nftw

    int (*rm_func)(const char *) = remove;
    return rm_func(path);
}
void rm(char cmd[512][512], int flag1, int flag2, int posn, int last) {
    //removes file, supports multi input
    //flags: -d (directory - delete empty dir) -r (recursive - delete non empty directory)

    if (last!=1) {
        cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    }

    int directory = 0;
    int recursive = 0;

    if (flag1!=-1){
        if (cmd[flag1][1]=='D' || cmd[flag1][1]=='d') {
            directory = 1;
        } else if (cmd[flag1][1]=='R' || cmd[flag1][1]=='r') {
            recursive = 1;
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    if (flag2!=-1) {
        if (cmd[flag2][1]=='D' || cmd[flag2][1]=='d') {
            directory = 1;
        } else if (cmd[flag2][1]=='R' || cmd[flag2][1]=='r') {
            recursive = 1;
        } else {
            printf("Error: Invalid flag.\n");
            return;
        }
    }
    
    int removeResult=0;
    int i = posn;
    int multiple = 1;
    if (posn >= (last-1)) {
        multiple = 0;
    }
    if (recursive==0) {
        for (i; i<last; i++){
            removeResult = remove(cmd[i]);

            if (removeResult!=0 & multiple==1) {
                printf("Error encountered while deleting %s\n", cmd[i]);
                break;
            }
        }
    } else {
        for (i; i<last; i++){
            removeResult = nftw(cmd[i], remover, FOPEN_MAX, FTW_DEPTH);

            if (removeResult!=0 & multiple==1) {
                printf("Error encountered while deleting %s\n", cmd[i]);
                break;
            }
        }
    }
    
    if (removeResult==0) {
        printf("Deleted.\n");
    } else if (recursive==0 && errno==39) { //remove sets errno=39 if directory is not empty
        //corner case: non empty directory entered without -r flag

        if (directory==1) {
            //edge case: directory flag entered, but path is non empty directory
            printf("Directory is non empty. Do you still wish to delete (Y [d]/N)? ");
            char userAns[256];
            fgets(userAns, sizeof(userAns), stdin);
            if (userAns[0]=='N' || userAns[0]=='n') {
                if (multiple == 0) {
                    printf("Exiting.\n");
                    return;
                } else {
                    if (i!=(last-1)){
                        rm(cmd, flag1, flag2, i+1, last);
                    }
                }
            }
        }

        if (multiple == 0){
            nftw(cmd[posn], remover, FOPEN_MAX, FTW_DEPTH); //FOPEN_MAX: max number of open files allowed, FTW_DEPTH: file tree walk depth
        } else {
            nftw(cmd[i], remover, FOPEN_MAX, FTW_DEPTH); //FOPEN_MAX: max number of open files allowed, FTW_DEPTH: file tree walk depth
        }
        printf("Non empty directory deleted.\n");

        if (multiple == 1) {
            if (i!=(last-1)){
                rm(cmd, flag1, flag2, i+1, last);
            }
        }
    } else {
        perror("Error");
        if (multiple == 1) {
            if (i!=(last-1)){
                printf("Skipping...\n");
                rm(cmd, flag1, flag2, i+1, last);
            }
        }
    }
}
