#include "shell.h"
#include "commands.h"
#include "pipes.h"
#include "bgproc.h"

int realSTDIN;
int realSTDOUT;
char pseudohome[MAXPATH];
int exitflag = 0;
int shellpid;
int prompt()
{
	char host[MAXPATH];
	char* user;
	char pwd[MAXPATH];
	
	//dealing with username
	user = getenv("USER");
	if (user==NULL)
	{
		printf("Unable to get username\n");
		exit(1);
	}

	//dealing with hostname
	if(gethostname(host, MAXPATH)!=0)
	{
		printf("Unable to get hostname\n");
		exit(1);
	}

	//dealing with pwd/cwd
	if(getcwd(pwd, MAXPATH)==NULL)
	{
		printf("Unable to get working directory\n");
		exit(1);
	}

	if(!strncmp(pwd,pseudohome,strlen(pseudohome)))
	printf("<%s@%s:~%s> ", user, host, &pwd[strlen(pseudohome)]);
	else
	//print the actual prompt
	printf("<%s@%s:%s> ", user, host, pwd);

	return 0;
}

int getinput()
{
	// read the exact line entered by the user
	char* initial_inputline;
	size_t temp = 0;
	if(getline(&initial_inputline, &temp, stdin)==-1)
	{
		exitflag = 1;
		endallchildren();
		exit(0);
	}
	
	// copy over the read line to an array, memleak otherwise
	char inputline[MAXCMD]="";
	strcpy(inputline, initial_inputline);
	free(initial_inputline);
	//printf("READ: |%s|\n", inputline);

	// execute the ';' separated commands
	char *token;
	char *rem = inputline;
	while ((token = strtok_r(rem, ";", &rem)))
	{
		piping(token);
		resetStds();
	}
	initial_inputline = NULL;
	strcpy(inputline, " ");
	return 0;
}

void ctrlc(int id) 
{
    return;
}

void ctrlz() 
{
    if(getpid()==shellpid)
	{
        return;
	}
    kill(getpid(), SIGTSTP);

}

int main()
{
	shellpid = getpid();
	
	realSTDIN = dup(STDIN_FILENO);
    realSTDOUT = dup(STDOUT_FILENO);

	//setting the pseudo-home to be the directory prash is invoked from
	if (getcwd(pseudohome, MAXPATH) == NULL)
	{
		printf("Unable to get working directory\n");
		exit(1);
	}
	
	signal(SIGCHLD, (void*)endcurrchild);
    signal(SIGINT, ctrlc);
    signal(SIGTSTP, ctrlz);
	signal(SIGQUIT, SIG_IGN);
	//after this, the 'shell' is basically just an infinite loop of prompts and input handling
	for (;;)
	{
		prompt();
		getinput();
	}
}
