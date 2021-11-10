#include "globalheader.h"
#include "commands.h"
extern int realSTDIN;
extern int realSTDOUT;



void resetStds() 
{
	dup2(realSTDIN, STDIN_FILENO);
	dup2(realSTDOUT, STDOUT_FILENO);
}

void run(char* rececommand, int in, int out) 
{
	int forkvalhere = fork();
	if(forkvalhere==0)
	{
		if(in != 0)
        {
			dup2(in, 0); 
			close(in);
		}
        if(out != 1)
        {
			dup2(out, 1);
			close(out);
		}
		

		char command[MAXCMD] = "";
		strcpy(command, rececommand);
		char parsedcommand[MAXCMD] = " ";
		char *token;
		char *rem = command;
		int argnum=0;
		//whitespace tokenization
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

		char *command2d[argnum + 1];
		rem = parsedcommand;
		for (int i = 0; (token = strtok_r(rem, " ", &rem)); i++)
		{
			command2d[i] = token;
		}
		command2d[argnum] = NULL;
		if (execvp(command2d[0], command2d) == -1)
		{
			char tempstr[1000] = " ";
			sprintf(tempstr,"%s: command not found\n", command2d[0]);
			write(2, tempstr, strlen(tempstr));
			exit(1);
		}
	}
	int status;
    waitpid(forkvalhere, &status, WUNTRACED);
}

int piping(char rececommand[]) 
{
	/*printf("rececommand:((%s))\n", rececommand);*/
	int nooftokens = 1;
	// count number of segments divided by pipes
	for (int j = 0; j < strlen(rececommand); j++)
		if(rececommand[j]=='|')
			nooftokens++;

	//tokenize received command on basis of pipes to get segments and form an array of segments.
	char *cmd[nooftokens];
	char *rem = rececommand;
	char *token;
	for (int j = 0; (token = strtok_r(rem, "|", &rem)); j++)
	{
		cmd[j] = token;
	}

	int fd[2];
    int outf = 1;
    int inf = 0;
    int appf = 1;

	resetStds();
	/*for (int i = 0; i < (nooftokens); i++)
	{
		printf("token %d:((%s))\n",i, cmd[i]);
	}*/
	for (int i = 0; i < (nooftokens - 1); i++)
	{
		char inner_parsedcommand[MAXCMD] = " ";
		rem = cmd[i];
		for (; (token = strtok_r(rem, " \t\n", &rem));)
		{
			strcat(inner_parsedcommand, token);
			strcat(inner_parsedcommand, " ");
		}
		if(i==0)
		{
			///////INNER/////////
			//checking for input redir for first token
			int inner_r_inflag = 0, inner_r_index;
			char inner_r_inpath[MAXPATH] = " ";
			
			//search for redir operators, update flags and indices of input and output paths
			for (int m = 0;m<strlen(inner_parsedcommand); m++)
			{
				if (inner_parsedcommand[m]=='<')
				{
					inner_r_inflag++;
					inner_r_index = m + 2;
				}
			}
			if(inner_r_inflag==1)
			{
				//get the first string (output path), starting from index
				char *rem = inner_parsedcommand + inner_r_index;
				strcpy(inner_r_inpath,strtok_r(rem, " ", &rem ));
				//printf("INPATH:%s\n", r_inpath);
				inf = open(inner_r_inpath, O_RDONLY, 0644);
			}
		}
		////////END OF INNER////////
		if (pipe(fd) == -1)
      		printf("Error in forming pipe\n");
		run(inner_parsedcommand, inf, fd[1]);
		close(fd[1]);
		inf = fd[0];
	}
	/////////////////////////
	//for the last command, or a command with no pipes, handle redir
	//whitespace tokenization
	int r_outflag=0, r_outflag2=0, r_outdex;
	int r_inflag = 0, r_index;
	char r_inpath[MAXPATH] = " ",r_outpath[MAXPATH] = " ";
	char parsedcommand[MAXCMD] = " ";
	rem = cmd[nooftokens - 1];
	for (; (token = strtok_r(rem, " \t\n", &rem));)
	{
		strcat(parsedcommand, token);
		strcat(parsedcommand, " ");
	}
	//search for redir operators, update flags and indices of input and output paths
	for (int i = 0;i<strlen(parsedcommand); i++)
	{
		if (parsedcommand[i]=='>')
		{
			if(parsedcommand[i+1]=='>')
			{
				r_outflag2++;
				r_outdex = i + 3; //because '>> path'
				i++;
			}
			else
			{
				r_outflag++;
				r_outdex = i + 2;
			}
		}
		if (parsedcommand[i]=='<')
		{
			r_inflag++;
			r_index = i + 2;
		}
	}
	if(r_outflag==1||r_outflag2==1)
	{
		//get the first string (output path), starting from outdex
		char *rem = parsedcommand + r_outdex;
		strcpy(r_outpath,strtok_r(rem, " ", &rem ));
		//printf("OUTPATH:%s\n", r_outpath);
		if(r_outflag==1) // >
			outf = open(r_outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		else // >>
			appf = open(r_outpath, O_WRONLY | O_CREAT | O_APPEND, 0644);
	}
	if(r_inflag==1)
	{
		//get the first string (output path), starting from index
		char *rem = parsedcommand + r_index;
		strcpy(r_inpath,strtok_r(rem, " ", &rem ));
		//printf("INPATH:%s\n", r_inpath);
		inf = open(r_inpath, O_RDONLY, 0644);
	}
	///////////////////////
	//in out fd switcheroonie
	if(inf != 0){dup2(inf, 0);}
    if(outf != 1){dup2(outf, 1);}
    if(outf == 1 && appf != 1){dup2(appf, 1);}
	//printf("Calling Handle |%s|\n", parsedcommand);
	handlecommand(parsedcommand);
	if(inf)close(inf);
    if(outf != 1){close(outf);}
    if(appf != 1){close(appf);}
    return 0;
}