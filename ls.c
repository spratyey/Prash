#include "ls.h"

int aflag = 0;
int lflag = 0;

int lsfile(char* filepath)
{
	//if the -l flag is not present, just print the name of the entry and move on
	if(!lflag)
	{
		printf("%s\n", filepath);
	}
	else
	{
		// form the stat struct
		struct stat details;
		stat(filepath, &details);

		//get perms
		char perms[20] = "-";
		strcat(perms, (details.st_mode & S_IRUSR) ? "r" : "-");
		strcat(perms, (details.st_mode & S_IWUSR) ? "w" : "-");
		strcat(perms, (details.st_mode & S_IXUSR) ? "x" : "-");
		strcat(perms, (details.st_mode & S_IRGRP) ? "r" : "-");
		strcat(perms, (details.st_mode & S_IWGRP) ? "w" : "-");
		strcat(perms, (details.st_mode & S_IXGRP) ? "x" : "-");
		strcat(perms, (details.st_mode & S_IROTH) ? "r" : "-");
		strcat(perms, (details.st_mode & S_IWOTH) ? "w" : "-");
		strcat(perms, (details.st_mode & S_IXOTH) ? "x" : "-");

		//get other ls -l data
		int entries = details.st_nlink;
		struct passwd* owner = getpwuid(details.st_uid);
		struct group* group = getgrgid(details.st_gid);
		long long int size = details.st_size;

		//get date 
		char date[50];
		char temp[20];
		strftime(date, 30, "%b %d", localtime(&(details.st_mtime)));
		time_t t;
		if(time(&t)-details.st_mtime < 1578000) // if the file was created >=6 months ago, change date format
			strftime(temp, 20, "  %H:%M", localtime(&(details.st_mtime)));
		else
			strftime(temp, 20, "  %Y", localtime(&(details.st_mtime)));
		strcat(date, temp);

		//print the whole ls -l for the file under consideration
		printf("%-14s%-6d%-10s%-10s%-10lld%-18s%-20s\n", perms, entries, owner->pw_name, group->gr_name, size, date, filepath);
	}
	return 0;
}

int lsdir(char* dirpath)
{
	//printf("AFLAG: %d, LFLAG: %d\n", aflag, lflag);
	struct dirent **read;
	int noofentries = scandir(dirpath, &read, NULL, alphasort);

	// dirent read[] array contains ONLY the name of the file INSIDE the dir. This confuses stat() because the NAME alone is useless without the context of the cwd/path from cwd.
	char statpath[strlen(dirpath) + MAXPATH];
	struct stat existence;
	strcpy(statpath, dirpath);
	if(stat(statpath, &existence)!=0)
	{
		printf("ls: cannot access path: No such file or directory\n");
		return 1;
	}
	else if(S_ISREG(existence.st_mode))
	{
		lsfile(statpath);
		return 0;
	}
	
	
	//get 'total'
	if (lflag)
	{
		long long int tot = 0;
		for (int i = 0; i < noofentries; i++)
		{
			if(read[i]->d_name[0]=='.'&&!aflag)
				continue;
			strcpy(statpath, dirpath);
			strcat(statpath, "/");
			strcat(statpath, read[i]->d_name);
			struct stat details;
			stat(statpath, &details);
			tot+=details.st_blocks;
		}
		printf("total %lld\n", tot/2);
	}

	// iterate through all dir entries
	for (int i = 0; i < noofentries; i++)
	{
		//handling the -a flag
		if(read[i]->d_name[0]=='.'&&!aflag)
			continue;
		
		//if the -l flag is not present, just print the name of the entry and move on
		if(!lflag)
		{
			printf("%s\n", read[i]->d_name);
		}
		else
		{
			// form the stat struct
			strcpy(statpath, dirpath);
			strcat(statpath, "/");
			strcat(statpath, read[i]->d_name);
			struct stat details;
			stat(statpath, &details);

			//get perms
			char perms[20] = "";
			strcat(perms, (S_ISDIR(details.st_mode)?"d":"-"));
			strcat(perms, (details.st_mode & S_IRUSR) ? "r" : "-");
			strcat(perms, (details.st_mode & S_IWUSR) ? "w" : "-");
			strcat(perms, (details.st_mode & S_IXUSR) ? "x" : "-");
			strcat(perms, (details.st_mode & S_IRGRP) ? "r" : "-");
			strcat(perms, (details.st_mode & S_IWGRP) ? "w" : "-");
			strcat(perms, (details.st_mode & S_IXGRP) ? "x" : "-");
			strcat(perms, (details.st_mode & S_IROTH) ? "r" : "-");
			strcat(perms, (details.st_mode & S_IWOTH) ? "w" : "-");
			strcat(perms, (details.st_mode & S_IXOTH) ? "x" : "-");

			//get other ls -l data
			int entries = details.st_nlink;
			struct passwd* owner = getpwuid(details.st_uid);
			struct group* group = getgrgid(details.st_gid);
			long long int size = details.st_size;

			//get date 
			char date[50];
			char temp[20];
			strftime(date, 30, "%b %d", localtime(&(details.st_mtime)));
			time_t t;
			if(time(&t)-details.st_mtime < 1578000) // if the file was created >=6 months ago, change date format
				strftime(temp, 20, "  %H:%M", localtime(&(details.st_mtime)));
			else
				strftime(temp, 20, "  %Y", localtime(&(details.st_mtime)));
			strcat(date, temp);

			//print the whole ls -l for the dir entry under consideration
			printf("%-14s%-6d%-10s%-10s%-10lld%-18s%-20s\n", perms, entries, owner->pw_name, group->gr_name, size, date, read[i]->d_name);
		}
	}

	// scandir internally uses malloc. so now free the mem.
	for (int i = 0; i < noofentries;i++)
	{
		free(read[i]);
	}
	free(read);

	return 0;
}

int ls(char command[], int argnum)
{
	aflag = lflag = 0;
	char *token;
	char *rem = command;
	char paths[argnum][MAXPATH];
	int i;

	//parsing options, making list of paths
	for (i = 0; (token = strtok_r(rem, " ", &rem));)
	{
		//printf("TOKEN: |%s|\n", token);
		//checking for l option
		if(!strcmp(token,"-l")||!strcmp(token,"-al")||!strcmp(token,"-la"))
			lflag = 1;

		//checking for a option
		if(!strcmp(token,"-a")||!strcmp(token,"-al")||!strcmp(token,"-la"))
			aflag = 1;

		//checking for path (for now, consider any arg that is not an option to be a path)
		if(strcmp(token,"-a")&&strcmp(token,"-l")&&strcmp(token,"-al") && strcmp(token,"-la"))
		{
			strcpy(paths[i++], token);
		}
	}
	//i is the number of paths now


	//dealing with ~ in paths
	for (int j = 1; j < i;j++) //paths[0] is 'ls' itself
	{
		if(paths[j][0]=='~')
		{
			if(strlen(paths[j])>1)
			{
				char temp[strlen(pseudohome) + strlen(paths[j])];
				strcpy(temp, pseudohome);
				strcat(temp, paths[j]+1);
				strcpy(paths[j], temp);
			}
			else
			{
				strcpy(paths[j], pseudohome);
			}
		}
	}
	
	// no path(s) provided to ls => do for cwd
	if(i==1)
		lsdir(".");
	
	// for every path (except paths[0] because that is ls itself), do an lsdir
	for (int j = 1; j < i ;j++) 
	{
		if(i>2)
			printf("\n%s:\n",paths[j]);
		lsdir(paths[j]);
	}
	
	return 0;
}