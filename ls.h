#ifndef LS_H
#define LS_H
#include "globalheader.h"

int ls(char command[], int argnum);
int lsdir(char *dirpath);
int lsfile(char *filepath);

extern int lflag, aflag;

#endif