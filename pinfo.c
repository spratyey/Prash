#include "pinfo.h"

int pinfo(char command[], int argnum)
{
	if(argnum>2)
	{
		printf("pinfo: Invalid syntax. Usage: pinfo <pid>.\n");
		return 0;
	}
	else
	{

		char *token;
		char *rem = command;
		int inputpid = 0;
		if (argnum == 1)
			inputpid = getpid();
		else
		{
			for (int i = 1; (token = strtok_r(rem, " ", &rem)); i++)
			{
				if (i == 2)
					break;
			}
			inputpid=atoi(token);
		}


		char path[MAXPATH]; 
        char buf[MAXPATH];

		char procstatus;
		int temp1, temp2, temp3, temp4;
		int quantity1, quantity2;
		long unsigned mem;
		sprintf(path, "/proc/%d/stat", inputpid);
		
        FILE* fp = fopen(path, "r");
        if(!fp) 
		{
			printf("Error in obtaining process stats\n"); 
        }
        else 
		{
			printf("pid -- %d\n", inputpid);
			fscanf(fp, "%d %s %c %d %d %d %d %d", &temp1, buf, &procstatus,&temp2,&quantity1,&temp3,&temp4,&quantity2);
			printf("Process Status -- %c", procstatus);
			if(quantity1==quantity2)
				printf("+\n");
			else
			{
				printf("\n");
			}
			for(int i=0;i<14;i++) //23rd overall value=14th after the previous fscanf
           	 {
					fscanf(fp, "%s", buf);
					//printf("%d,|%s|", i, buf);
			 }
			fscanf(fp, "%lu", &mem);
			printf("memory -- %lu\n", mem);
		}

		sprintf(path, "/proc/%d/exe", inputpid);
		if(readlink(path, buf, MAXPATH)==-1)
		{
        	printf("Error in obtaining process executable path.\n");
			return 0;
		}
		
		char newexe[MAXPATH]="~";
		if (!(strncmp(buf, pseudohome, strlen(pseudohome))))
		{
			strcat(newexe, buf + strlen(pseudohome));
		}
		else
		{
			strcpy(newexe, buf);
		}
		printf("Executable Path -- %s\n", newexe);
	}
	return 0;
}