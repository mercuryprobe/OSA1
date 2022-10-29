int* flagger(char splitString[512][512], int argLen) {
    int flag1 = -1;
    int flag2 = -1;

    int flag1Taken = 0;
    int flag2Taken = 0;
    int i=0;
    int thread = 0;
    for (i; i<argLen; i++) {
        // printf("%d\n", i);
        if (strcmp(splitString[i], "&t")==0) {
            thread = 1;
        }
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
    int flags[] = {flag1, flag2, flag1Taken, flag2Taken, thread};
    return flags;
}