#define _XOPEN_SOURCE 500 
#define __USE_XOPEN_EXTENDED 500
#include <ftw.h>
#include <limits.h>
#include <sys/stat.h> 

void rm(char cmd[512][512], int flag1, int flag2, int posn, int last);
int remover(const char *path, const struct stat *s, int flag, struct FTW *ftw);