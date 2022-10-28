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

//date
// #define _XOPEN_SOURCE 1 
#include <time.h>

//cat
#include <signal.h>

//ls
#include <sys/types.h>
#include <dirent.h>


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

void mkdir_(char cmd[512][512], int flag1, int flag2, int posn, int last) {
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
        if (posn>=last) {
            //edge case: no dirname entered
            printf("Error: Missing operand\n");
        }
        return;
    } else {
        perror("Error");
        if (multiple==1 && (i!=last)) {
            //if multiple dirs have been mentioned
            printf("Skipping...\n");
            mkdir_(cmd, flag1, flag2, i+1, last+1);
        }
    }
}

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

            printf("%s\n", input);
            // strptime(input, format, inpTime);
            time_t time = mktime(inpTime);
            printf(ctime(&time));
        }
    }
}

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

static int a = 0;
int lister(const char *path, const struct stat *s, int flag, struct FTW *ftw) {
    // lister function, run on every node (directory) during file tree walk by nftw

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
            //print files in a directory
            if ((a==1 && ((dirStruc->d_name)[0]=='.')) || ((dirStruc->d_name)[0]!='.')) {
                printf("%s  ", dirStruc->d_name);
            }
            dirStruc = readdir(directory);
        }
        printf("\n\n");
    }
    return 0;
}
void ls(char cmd[512][512], int flag1, int flag2, int posn, int last) {
    cmd[last-1][strcspn(cmd[last-1], "\n")]=0;

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
    printf("%s\n", last);
    if (r==0) {
        //non recursive flag
        DIR *directory;
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

void shell() {
    // cd echo pwd
    // ls cat date rm mkdir
    while (1)
    {    printf("[rmnShell]$ ");
        
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
            if (splitString[i][0] == '-' || ((strcmp(splitString[0], "cat")==0) && (splitString[i][0] == '>'))) {
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
        } else if (strcmp(splitString[0], "mkdir")==0) {
            mkdir_(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen);
        } else if (strcmp(splitString[0], "date")==0) {
            date_(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 
        } else if (strcmp(splitString[0], "cat")==0) {
            cat(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 
        } else if (strcmp(splitString[0], "ls")==0) {
            ls(splitString, flag1, flag2, 1 + flag1Taken + flag2Taken, argLen); 
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