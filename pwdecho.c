#include "pwdecho.h"

int pwd(char command[], int argnum)
{
	char pwd[MAXPATH];

	if(argnum==1)
	{
		if(getcwd(pwd, MAXPATH)==NULL)
		{
			printf("Unable to get working directory\n");
			return 1;
		}
		printf("%s\n", pwd);
	}
	else
	{
		printf("pwd: Invalid syntax. Usage: pwd");
		return 1;
	}

	return 0;
}

int echo(char command[], int argnum)
{
	char *rem = command;
	strtok_r(rem, " ", &rem);

	printf("%s\n", rem);
	return 0;
}