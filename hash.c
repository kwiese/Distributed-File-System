#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>


#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct clientSock
{
	int *socket1;
	int *socket2;
	int *socket3;
	int *socket4;
};

char usernames[20][1024];
char passwords[20][1024];
char uname[1][1024];
char password[1][1024];
char ip[4][1024];
int port[4];
char path[8192];
char *DFS = "DFST";

char **getTypes(char stringToSplit[])
{
	//splits up string based on spaces
	char **type = malloc(10*sizeof *type);
	const char d[] = " ";
	char *token;
	token = strtok(stringToSplit, d);
	int i = 0;
	while(token != NULL)
	{
		type[i] = token;
		token = strtok(NULL, d);
		i++;
	}
	return type;
}
char **getTypes2(char stringToSplit[])
{
	//splits up string based on spaces
	char **type = malloc(2*sizeof *type);
	const char d[] = ",";
	char *token;
	token = strtok(stringToSplit, d);
	int i = 0;
	while(token != NULL)
	{
		type[i] = token;
		token = strtok(NULL, d);
		i++;
	}
	return type;
}
int fileSize(int fd)
{
	//grabs the file size (allows for sending files)
	struct stat stat_struct;
	if (fstat(fd, &stat_struct) == -1)
		return (-1);
		
	return (int) stat_struct.st_size;
}

char *getFileName(char *header)
{
	const char *filePat = "File: ";
	const char *userPat = ",User: ";
	char *start, *end;
	
	char *filename = NULL;
	if((start = strstr(header, filePat)))
	{
		start += strlen(filePat);
		if((end = strstr(header, userPat)))
		{
			filename = (char*)malloc(end - start + 1);
			memcpy(filename, start, end - start);
			filename[end-start]='\0';
		}
	}
	
	return filename;
}

char *getFunction(char *header)
{
	const char *funcPat = ",Function: ";
	char *start, *end;
	char *function = NULL;
	if((start = strstr(header, funcPat)))
	{
		start += strlen(funcPat);
		if((end = strchr(header, '\0')))
		{
			function = (char*)malloc(end - start + 1);
			memcpy(function, start, end - start);
			function[end-start]='\0';
		}
	}
	return function;
}

char *splitHeader(char *header)
{
	const char *filePat = "File: ";
	const char *userPat = ",User: ";
	const char *passPat = ",Password: ";
	char *start, *end;
	char *filename = NULL;
	if((start = strstr(header, filePat)))
	{
		start += strlen(filePat);
		if((end = strstr(header, userPat)))
		{
			filename = (char*)malloc(end - start + 1);
			memcpy(filename, start, end - start);
			filename[end-start]='\0';
		}
	}
	if(filename) printf("File: %s\n", filename);
	
	char *username = NULL;
	if((start = strstr(header, userPat)))
	{
		start += strlen(userPat);
		if((end = strstr(header, passPat)))
		{
			username = (char*)malloc(end - start + 1);
			memcpy(username, start, end - start);
			username[end-start]='oid makeDFSDir()
{
	if(getcwd(path, sizeof(path)) != NULL);
	char checkDir[1024];
	sprintf(checkDir, "%s/%s/" , path, DFS);
	struct stat dt = {0};
	if (stat(checkDir, &dt) == -1) {
		mkdir(checkDir, 0007);
	}
}

void parseConf()
{
	char *lines[20];
	char line[256];
	char *pos;
	FILE *fp = fopen("dfs.conf", "r");
	char **info;
	char path[1024];
	if(getcwd(path, sizeof(path)) != NULL);
	if(fp == NULL)
	{
		printf("error");
		exit(-1);
	}
	int j = 0;
	int numlines = 0;
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		lines[j] = strdup(line);
		j++;
		numlines++;
	}
	fclose(fp);
	int i = 0;
	j = 0;
	for(i = 0; i < numlines; i++)
	{
		info = getTypes(lines[i]);
		strcpy(usernames[j], info[0]);
		strcpy(passwords[j], info[1]);
		if((pos = strchr(passwords[j], '\n')) != NULL)
		 {
			 *pos = '\0';
		 }
		char dir[1024];
		sprintf(dir, "%s/%s/" , path, DFS);
		strcat(dir, usernames[j]);
		struct stat st = {0};
		if (stat(dir, &st) == -1) {
			mkdir(dir, 0007);
		}
		j++;
	}
}

int check(char *user, char *pass)
{
	inar **info;
	if(fp == NULL)
	{
		printf("error");
		exit(-1);
	}
	int j = 0;
	int numlines = 0;
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		lines[j] = strdup(line);
		j++;
		numlines++;
	}
	fclose(fp);
	int i = 0;
	j = 0;
	for(i = 0; i < numlines; i++)
	{
		info = getTypes(lines[i]);
		strcpy(usernames[j], info[0]);
		strcpy(passwords[j], info[1]);
		if((pos = strchr(passwords[j], '\n')) != NULL)
		{
			 *pos = '\0';
		}
		char dir[1024];
		sprintf(dir, "%s" , "/home/user/Dropbox/Networking_Systems/PA2/DFS1/");
		strcat(dir, usernames[j]);

		struct stat st = {0};
		if (stat(dir, &st) == -1) {
			mkdir(dir, 0007);
		}
		j++;
	}
	puts(usernames[0]);
	puts(passwords[0]);
	
	char writefile[8192];
	char writefile1[8192];
	char fileu1[1024];
	char filen1[1024];
	char *filename1 = "simpletxt1.txt";
	char *fileusr1 = usernames[0];
	sprintf(writefile, "%s" , "/home/user/Dropbox/Networking_Systems/PA2/DFST/");
	strcat(writefile, usernames[0]);

	struct stat st = {0};
	if (stat(writefile, &st) == -1) {
		mkdir(writefile, 0007);
	}
	sprintf(writefile1, "%s" , "/home/user/Dropbox/Networking_Systems/PA2/DFST/");
	sprintf(filen1, "%s", filename1);
	sprintf(fileu1, "%s/", fileusr1);
	strcat(writefile1, fileu1);
	strcat(writefile1, filen1);
	puts(writefile1);
	FILE *pfile1 = fopen(writefile1, "w");
	fclose(pfile1);
	
	
	
	FILE *fp;
	char writefile1[8192];
	char filestr[8192];
	char dir[8192];
	sprintf(writefile1, "%s" , "/home/user/Dropbox/Networking_Systems/PA2/DFST");
	char *filen;
	char *fileu;
	
	char *msg = "File: sampletxt1.txt,User: Kyle,Password: testpass";
	filen = getFileName(msg);
	fileu = getUserName(msg);
	sprintf(filestr, "%s", filen);
	sprintf(dir, "%s", fileu);
	strcat(writefile1, dir);
	char *test;
	strcpy(test, writefile1);
	puts(filestr);
	struct stat st = {0};
	puts(test);
	if (stat(test, &st) == -1) {
		mkdir(test, 0000);
	}
	//puts(writefile1);
	//fp = fopen(writefile1, "w");
	//fclose(fp);
	
	//char *password;
	
	int sock1 = 1;
	int sock2 = 2;
	int sock3 = 3;
	int sock4 = 4;
	//password = splitHeader(msg);
	//printf("%s\n", password);

	struct clientSock *clisocket = malloc(sizeof(struct clientSock));
	clisocket->socket1 = &sock1;
	clisocket->socket2 = &sock2;
	clisocket->socket3 = &sock3;
	clisocket->socket4 = &sock4;
	
	testPass(*clisocket);*/
	
	/*int k, resourcefd, length;
	FILE *fp;
	unsigned char buff[1024];
	resourcefd = open("sampletxt.txt", O_RDONLY, 0);
	length = fileSize(resourcefd);
	close(resourcefd);
	k = length / 4;
	
	fp = fopen("sampletxt.txt", "r");
	int total = 0;
	while(!feof(fp))
	{
		fread(buff, sizeof(unsigned char), 1, fp);
		unsigned char *line = buff;
		if(total <= k)
		{
			total += 1;
			printf("%s", line);
		}
		else
		{
			break;
		}
	}
	fclose(fp);*/
	return 0;	
}


