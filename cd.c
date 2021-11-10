#include "cd.h"
char OLDPWD[MAXPATH]="";
int cd(char command[], int argnum)
{
	// rule: OLDPWD is to be set to the value of the pwd_before_successful-cd-execution 
	char temp_oldpwd[MAXPATH];
	getcwd(temp_oldpwd, MAXPATH);

	// only cd
	if (argnum == 1)
	{
		if(chdir(pseudohome))
		{
			perror("cd: Error");
			return 1;
		}
		else
		{
			strcpy(OLDPWD, temp_oldpwd);
		}
	}

	// cd path
	else if(argnum==2)
	{
		char *token;
		char *rem = command;
		for (int i = 1; (token = strtok_r(rem, " ", &rem)); i++)
		{
			if(i==2)
				break;
		}
		//printf("PATH: |%s|, OLDPWD: |%s|\n", token,OLDPWD);

		//if path begins with ~
		if(token[0] == '~')
		{
			char newtoken[strlen(pseudohome) + strlen(token)];
			strcpy(newtoken, pseudohome);
			strcat(newtoken, token+1);
			if(chdir(newtoken))
			{
				perror("cd: Error");
				return 1;
			}
			else
			{
				strcpy(OLDPWD, temp_oldpwd);
			}
			
		}

		// if the command is 'cd -'
		else if(!(strncmp(token, "-",1)))
		{
			if(chdir(OLDPWD))
			{
				perror("cd: Error");
			}
			else
			{
				strcpy(OLDPWD, temp_oldpwd);
			}
		}

		// some other path
		else 
		{
			if(chdir(token))
			{
				perror("cd: Error");
			}
			else
			{
				strcpy(OLDPWD, temp_oldpwd);
			}
		}
	}

	// >2 'words' in the command
	else
	{
		printf("cd: Invalid syntax. Usage: cd <path>.\n");
	}

	return 0;
}