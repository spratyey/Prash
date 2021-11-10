#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <fcntl.h>

#include <errno.h>
#include <assert.h>

#define MAXPATH 4096
#define MAXCMD 8192

extern char pseudohome[MAXPATH];

extern int exitflag;
