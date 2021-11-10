#include "commands.h"
#include "cd.h"
#include "pwdecho.h"
#include "ls.h"
#include "bgproc.h"
#include "pinfo.h"

struct bgproc bgproctable[100];
int bgproccount = 0;

int handlecommand(char rececommand[])
{	
	
	char command[MAXCMD] = "";
	strcpy(command, rececommand);
	//printf("in handelcommand, received |%s|\n", command);
	//definitions for dealing with whitespaces
	char parsedcommand[MAXCMD] = " ";
	char *token;
	char *rem = command;
	int andflag = 0;

	

	//whitespace tokenization
	int argnum=0;
	for (; (token = strtok_r(rem, " \t\n", &rem));)
	{
		strcat(parsedcommand, token);
		strcat(parsedcommand, " ");
	}
	
	//edit out the redirection characters from parsedcommand
	char tempc[MAXCMD] = " ";
	
	for (int i = 0; i < strlen(parsedcommand);i++)
	{
		char ch = parsedcommand[i];
		if(ch!='>'&& ch!='<')
		{
			tempc[i] = parsedcommand[i];
		}
		else
		{
			tempc[i] = '\0';
			break;
		}
	}
	strcpy(parsedcommand, tempc);
	rem = tempc;
	for (argnum = 0; (token = strtok_r(rem, " \t\n", &rem)); argnum++)
		;
	// now, parsedcommand is a sequence of words separated by single spaces. there is also one leading and one trailing space. argnum is the number of these words.

	//printf("PARSED: |%s|, COUNT: %d\n", parsedcommand,argnum);

	//exit/quit command
	if (!(strcmp(parsedcommand, " exit ")) || !(strcmp(parsedcommand, " quit ")))
	{
		exitflag = 1;
		endallchildren();
		exit(0);
	}

	//repeat command
	else if (!(strncmp(parsedcommand, " repeat ", 8)))
	{
		int numiter = 1;
		char newcommand[MAXCMD] = " ";
		char *temptoken;
		char *temprem = parsedcommand;
		for (int k = 1; (temptoken = strtok_r(temprem, " ", &temprem)); k++)
		{
			if (k == 2)
				numiter = atoi(temptoken);
			if (k > 2)
			{
				strcat(newcommand, temptoken);
				strcat(newcommand, " ");
			}
		}
		//printf("NEWCOMMAND=|%s|\n",newcommand);
		char copy2[MAXCMD] = "";
		strcpy(copy2, newcommand);
		for (int iter = 1; iter <= numiter; iter++)
		{
			handlecommand(copy2);
		}
	}

	//cd
	else if (!(strncmp(parsedcommand, " cd ", 4)))
	{
		cd(parsedcommand, argnum);
	}

	//pwd
	else if (!(strncmp(parsedcommand, " pwd ", 5)))
	{
		pwd(parsedcommand, argnum);
	}

	//echo
	else if (!(strncmp(parsedcommand, " echo ", 6)))
	{
		echo(parsedcommand, argnum);
	}

	//ls
	else if (!(strncmp(parsedcommand, " ls ", 4)))
	{
		ls(parsedcommand, argnum);
	}

	//pinfo
	else if (!(strncmp(parsedcommand, " pinfo ", 7)))
	{
		pinfo(parsedcommand, argnum);
	}

	//jobs
	else if (!(strncmp(parsedcommand, " jobs ", 6)))
	{
		printtable(parsedcommand, argnum);
	}

	//bg
	else if (!(strncmp(parsedcommand, " bg ", 4)))
	{
		bg(parsedcommand, argnum);
	}

	//fg
	else if (!(strncmp(parsedcommand, " fg ", 4)))
	{
		fg(parsedcommand, argnum);
	}

	//sig
	else if (!(strncmp(parsedcommand, " sig ", 5)))
	{
		sig(parsedcommand, argnum);
	}

	// system commands
	else if (argnum >= 1)
	{
		andflag = 0;

		if (!strcmp(parsedcommand + strlen(parsedcommand) - 3, " & "))
		{
			andflag = 1;
		}

		char *command2d[argnum + 1];
		rem = parsedcommand;
		for (int i = 0; (token = strtok_r(rem, " ", &rem)); i++)
		{
			command2d[i] = token;
		}
		command2d[argnum] = NULL;
		if (andflag)
			command2d[argnum - 1] = NULL;
		if (andflag && bgproccount >= 99)
		{
			printf("Background process limit reached. Aborting. \n");
			goto END;
		}
		int forkval = fork();
		if (forkval == 0) //we are in the child process
		{
			setpgid(0, 0);
			signal(SIGINT, SIG_DFL);
			if (execvp(command2d[0], command2d) == -1)
			{
				printf("%s: command not found\n", command2d[0]);
				exit(1);
			}
		}
		else if (forkval < 0)
		{
			printf("Failed to generate child process\n");
		}
		else //we are in the parent process
		{
			if (andflag)
			{
				printf("%d\n", forkval);
				setpgid(forkval, forkval);
				strcpy(bgproctable[bgproccount].procname, command2d[0]);
				bgproctable[bgproccount].procid = forkval;
				bgproctable[bgproccount++].exists = 1;
				updatesrnos();
			}
			else
			{
				signal(SIGTTIN, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                tcsetpgrp(0, forkval);
				tcsetpgrp(1, forkval);
				int status;
				do
				{
                    waitpid(forkval, &status, WUNTRACED);
                }while(!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));                
                
                tcsetpgrp(0, getpgid(0));
				tcsetpgrp(1, getpgid(0));
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
                if (WIFSTOPPED(status)) 
				{
                   strcpy(bgproctable[bgproccount].procname, command2d[0]);
					bgproctable[bgproccount].procid = forkval;
					bgproctable[bgproccount++].exists = 1;
					updatesrnos();                       
                }
			}
			return 0;
		}
	}
	END:
	return 0;
}
