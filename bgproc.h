#ifndef BG_H
#define BG_H
#include "globalheader.h"

struct bgproc{
	int srno;
	char procname[PATH_MAX];
	int procid;
	int exists; //1 for exists in table, 0 for does not exist
};
int indexof(int pid);
int endcurrchild();
int endallchildren();
int updatesrnos();
int printtable(char command[], int argnum);
int bg(char command[], int argnum);
int fg(char command[], int argnum);
int sig(char command[], int argnum);
#endif