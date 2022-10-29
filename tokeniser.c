#include <stdlib.h>
#include "splitStruc.h"

struct splitStruc tokenise(char userInp[512]) {
    //tokenise input
    const char space[2] = " ";
    char* tokenInput;
    tokenInput = strtok(userInp, space);

    struct splitStruc tokens;
    int i = 0;
    while (tokenInput!=NULL) {
        strcpy(tokens.splitString[i], tokenInput);
        tokenInput = strtok(NULL, space);
        i+=1;
    }
    tokens.argLen = i;

    while (i<512) {
        tokens.splitString[i][0] = 0;
        i+=1;
    }

    tokens.splitString[0][strcspn(tokens.splitString[0], "\n")]=0;
    
    return tokens;
}