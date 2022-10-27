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
#include <sys/stat.h>


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

    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;

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
            removeResult = nftw(cmd[posn], remover, FOPEN_MAX, FTW_DEPTH);

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


void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("[rmnShell]>");
        
        char userInp[512]; //will store user input
        fgets(userInp, sizeof(userInp), stdin);
        
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
        // printf("%d\n", argLen);

        while (i<512) {
            splitString[i][0] = 0;
            i+=1;
        }

        
        int flag1 = -1;
        int flag2 = -1;

        int flag1Taken = 0;
        int flag2Taken = 0;
        i=0;
        for (i; i<argLen; i++){
            // printf("%d\n", i);
            if (splitString[i][0] == '-') {
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
        // printf("%d\n", flag1);
        // printf("%d\n", flag2);
        
        splitString[0][strcspn(splitString[0], "\n")]=0;
        if ((strcmp(splitString[0], "exit")==0) || (strcmp(splitString[0], "e")==0)) {
            //exit
            puts("Exiting...");
            return;
        } else if (strcmp(splitString[0], "cd")==0) {
            cd(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken);
        } else if (strcmp(splitString[0], "pwd")==0) {
            pwd(splitString, flag1, flag2); 
        } else if (strcmp(splitString[0], "echo")==0) {
            echo(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);
        } else if (strcmp(splitString[0], "rm")==0) {
            rm(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);
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