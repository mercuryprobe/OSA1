#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

//rm
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
            }
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
            }
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
            }
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
    //reference[1]: https://stackoverflow.com/questions/70695049/nftw-remove-the-directory-content-without-removing-the-top-dir-itself
    //reference[2]: https://stackoverflow.com/questions/1149764/delete-folder-and-all-files-subdirectories
    //reference[3]: https://man7.org/linux/man-pages/man3/ftw.3.html

    if(flag != FTW_D) { //flag==FTW_D if path is a directory
        return remove(path);
    }
    return 0;
}
void rm(char cmd[512][512], int flag1, int flag2, int posn, int last) {
    //removes file

    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;
    printf("Deleting <%s>\n", cmd[posn]);
    int removeResult = remove(cmd[posn]);
    if (removeResult==0) {
        printf("Deleted.\n");
    } else if (errno==39) { //remove sets errno=39 if directory is not empty
        //corner case: non empty directory entered without -r flag
        nftw(cmd[posn], remover, OPEN_MAX, FTW_DEPTH) //OPEN_MAX: max number of open files allowed, FTW_DEPTH: file tree walk depth
    } else {
        perror("Error");
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