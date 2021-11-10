#include "bgproc.h"

extern struct bgproc bgproctable[100];
extern int bgproccount;

char getstatus(int id)
{
    char buf[MAXPATH];
    char c;
	int temp;
	sprintf(buf, "/proc/%d/stat", id);
	FILE* fp = fopen(buf, "r");
    if (fp) 
	{
        if(!fscanf(fp, "%d %s %c", &temp, buf, &c)) 
		{
            return -1;
        }
		else
		{
            return c;
        }
    }
	return -1;
}
int comparenames(const void *a,const void *b)
{	
	struct bgproc* A = (struct bgproc* )a;
  	struct bgproc* B = (struct bgproc* )b;
	return (strcmp( A->procname,B->procname));
}
int comparesrnos(const void *a,const void *b)
{	
	struct bgproc* A = (struct bgproc* )a;
  	struct bgproc* B = (struct bgproc* )b;
	return (A->srno-B->srno);
}

int updatesrnos()
{
	int tempsr = 1;
	for (int i = 0; i < bgproccount; i++)
	{
		if(bgproctable[i].exists)
			bgproctable[i].srno = tempsr++;
	}
	return 0;
}

int indexof(int pid)
{
	for (int i = 0; i < bgproccount;i++)
	{
		if(bgproctable[i].procid==pid&&bgproctable[i].exists)
			return i;
	}
	return -1;
}
int getpidfromsrno(int srnoinput)
{
	if(srnoinput<1 || srnoinput>bgproccount)
		return -1;
	for (int i = 0; i < bgproccount; i++)
	{
		if(bgproctable[i].srno==srnoinput&&bgproctable[i].exists)
			return bgproctable[i].procid;
	}
	return -1;
}

int endcurrchild()
{
	if(exitflag==1)
		return 0;
		
	int status;
	int pidtoend = waitpid(-1, &status, WNOHANG);
	if(pidtoend>0)
	{
		printf("%s with pid %d exited", bgproctable[indexof(pidtoend)].procname, pidtoend);
		if (WIFEXITED(status)) 
		{
			printf(", status=%d\n", WEXITSTATUS(status));
		} 
		else
		{
			printf("\n");
		}
		bgproctable[indexof(pidtoend)].exists = 0;
		updatesrnos();
	}
	return 0;

}

int endallchildren()
{
	for (int i = 0; i < bgproccount;i++)
	{
		if(bgproctable[i].exists)
		{
			kill(bgproctable[i].procid, SIGKILL);
			printf("%s with pid %d exited\n", bgproctable[i].procname, bgproctable[i].procid);
			bgproctable[i].exists = 0;
		}
	}
	exit(0);
}

// jobs command
int printtable(char command[], int argnum)
{
	updatesrnos();
	int existcount = 0;
	for (int i = 0; i < bgproccount; i++)
	{
		if(bgproctable[i].exists)
			existcount++;
	}
	if(existcount==0)
	{
		printf("No background process exists.\n");
		return 0;
	}
	int rflag = 0,sflag = 0;
	char *token;
	char *rem = command;
	//parsing options
	for (; (token = strtok_r(rem, " ", &rem));)
	{
		//printf("TOKEN: |%s|\n", token);
		//checking for r option
		if(!strcmp(token,"-r")||!strcmp(token,"-sr")||!strcmp(token,"-rs"))
			rflag = 1;

		//checking for a option
		if(!strcmp(token,"-s")||!strcmp(token,"-sr")||!strcmp(token,"-rs"))
			sflag = 1;
	}
	qsort(bgproctable, bgproccount, sizeof(struct bgproc), comparenames);
	for (int i = 0; i < bgproccount; i++)
	{
		if(bgproctable[i].exists)
		{
			int procstatus= getstatus(bgproctable[i].procid);
			char *wordstatus;
			if (procstatus=='R'||procstatus=='S'||procstatus=='D')
				wordstatus = "Running";
			else if (procstatus == 'T'||procstatus=='Z')
				wordstatus = "Stopped";
			else
				wordstatus = "Unknown";

			if(rflag && !sflag && strcmp(wordstatus,"Running"))
				continue;
			if(sflag && !rflag &&  strcmp(wordstatus,"Stopped"))
				continue;
			printf("[%d] %s %s [%d]\n", bgproctable[i].srno, wordstatus, bgproctable[i].procname, bgproctable[i].procid);
		}
	}
	qsort(bgproctable, bgproccount, sizeof(struct bgproc), comparesrnos);
	return 0;
}

//bg command
int bg(char command[], int argnum)
{
	if (argnum != 2)
	{
		printf("Usage: bg <job_serial_num>. Run the command 'jobs' to find the serial number.\n");
		return 0;
	}
	char *token;
	char *rem = command;
	int srnoinput;
	for (int i = 1; (token = strtok_r(rem, " ", &rem)); i++)
	{
		if(i==2)
		{
			srnoinput = atoi(token);
		}
	}
	int pidtouse = getpidfromsrno(srnoinput);
	if(pidtouse==-1)
	{
		printf("No background job corresponding to the given job number exists\n");
		return 0;
	}
	kill(pidtouse, SIGCONT);
	return 0;
}

//fg command
int fg(char command[], int argnum)
{
	if (argnum != 2)
	{
		printf("Usage: fg <job_serial_num>. Run the command 'jobs' to find the serial number.\n");
		return 0;
	}
	char *token;
	char *rem = command;
	int srnoinput;
	for (int i = 1; (token = strtok_r(rem, " ", &rem)); i++)
	{
		if(i==2)
		{
			srnoinput = atoi(token);
		}
	}
	int pidtouse = getpidfromsrno(srnoinput);
	if(pidtouse==-1)
	{
		printf("No background job corresponding to the given job number exists\n");
		return 0;
	}

	int indextouse = indexof(pidtouse);
	bgproctable[indextouse].exists = 0;
	setpgid(pidtouse, getpgid(0));
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(STDIN_FILENO, pidtouse);
	kill(pidtouse, SIGCONT);
	int status;
	do
	{
		waitpid(pidtouse, &status, WUNTRACED);
	}while(!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
	tcsetpgrp(STDIN_FILENO, getpgrp());
	
	if (WIFSTOPPED(status))
	{
		if (pidtouse != -1)
			bgproctable[indextouse].exists = 1;
	}

	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);


	return 0;
}
int sig(char command[], int argnum)
{
	if (argnum != 3)
	{
		printf("Usage: sig <job_serial_num> <signal_num>. Run the command 'jobs' to find the serial number.\n");
		return 0;
	}
	char *token;
	char *rem = command;
	int srnoinput;
	int signo;
	for (int i = 1; (token = strtok_r(rem, " ", &rem)); i++)
	{
		if(i==2)
		{
			srnoinput = atoi(token);
		}
		if(i==3)
		{
			signo = atoi(token);
		}
	}
	int pidtouse = getpidfromsrno(srnoinput);
	int indexno = indexof(pidtouse);
	if (pidtouse == -1)
	{
		printf("No background job corresponding to the given job number exists\n");
		return 0;
	}
	if(signo==SIGCONT && (getstatus(pidtouse)=='T'||getstatus(pidtouse)=='Z'))
	{
		char command[1000];
		sprintf(command, " bg %d ", srnoinput);
		bg(command,2);
		return 0;
	}
	if(signo==SIGCONT && (getstatus(pidtouse)=='R'||getstatus(pidtouse)=='S'||getstatus(pidtouse)=='D'))
	{
		char command[1000];
		sprintf(command, " fg %d ", srnoinput);
		fg(command,2);
		return 0;
	}
	if(kill(pidtouse, signo)<0)
	{
		printf("Error in sending signal.\n");
		return 0;
	}

	if(getstatus(pidtouse)==-1)
		bgproctable[indexno].exists = 0;

	return 0;
}
