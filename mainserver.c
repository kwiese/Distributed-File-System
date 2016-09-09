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

int port;
char *DFS;
char usernames[20][1024];
char passwords[20][1024];

struct clientSock
{
	int *sockets;
};

char **getTypes(char stringToSplit[])
{
	//splits up string based on spaces
	char **type = malloc(2*sizeof *type);
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
	char **type = malloc(3*sizeof *type);
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

void sendMsg(int fd, char *msg)
{
	//sends a message to the client
	int size = strlen(msg);
	int numBytesSent;
	do
	{
		//printf("Message:%s\n", msg);
		numBytesSent = send(fd, msg, size, MSG_NOSIGNAL);
		//printf("Bytes Sent: %d out of %d\n", numBytesSent, size);
		if(numBytesSent <= 0) break;
		size -= numBytesSent;
		msg += numBytesSent;
	}while(size > 0);
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

char *getUserName(char *header)
{
	const char *userPat = ",User: ";
	const char *passPat = ",Password: ";
	char *start, *end;
	char *username = NULL;
	if((start = strstr(header, userPat)))
	{
		start += strlen(userPat);
		if((end = strstr(header, passPat)))
		{
			username = (char*)malloc(end - start + 1);
			memcpy(username, start, end - start);
			username[end-start]='\0';
		}
	}
	return username;
}

char *getPassword(char *header)
{
	const char *passPat = ",Password: ";
	const char *funcPat = ",Function: ";
	char *start, *end;
	char *pass = NULL;
	if((start = strstr(header, passPat)))
	{
		start += strlen(passPat);
		if((end = strstr(header, funcPat)))
		{
			pass = (char*)malloc(end - start + 1);
			memcpy(pass, start, end - start);
			pass[end-start]='\0';
		}
	}
	return pass;
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
	int i = 0;
	while(usernames[i] != NULL && passwords[i] != NULL)
	{
		if(strcmp(user, usernames[i]) == 0 && strcmp(pass, passwords[i]) == 0)
		{
			return 1;
		}
	}
	return -1;
}

void put(int fd, char *header)
{
	char path[1024];
	if(getcwd(path, sizeof(path)) != NULL);
	char writefile1[1024];
	sprintf(writefile1, "%s/%s/" , path, DFS);
	char writefile2[1024];
	sprintf(writefile2, "%s/%s/" , path, DFS);
	char *fileusr1;
	char *fileusr2;
	char *filename1;
	char *filename2;
	char filen1[1024];
	char filen2[1024];
	char fileu1[1024];
	char fileu2[1024];
	char body1[8192];
	char body2[8192];
	char header2[8192];
	FILE *pfile1, *pfile2;
	filename1 = getFileName(header);
	fileusr1 = getUserName(header);
	if(strlen(filename1) > 0)
	{
		//send first ack
		sendMsg(fd, "Sender ACK");
	}
	else
	{
		filename1 = "tempfile.txt";
	}

	//wait for body 1
	while(recv(fd, body1, 8192,  MSG_DONTWAIT) <= 0)
	{
	} 
	//write body 1 to file 1
	sprintf(filen1, "%s", filename1);
	sprintf(fileu1, "%s/", fileusr1);
	strcat(writefile1, fileu1);
	strcat(writefile1, filen1);
	pfile1 = fopen(writefile1, "w");
	fprintf(pfile1, "%s", body1);
	char cp2[8192];
	while(recv(fd, cp2, 8192, MSG_DONTWAIT) > 0)
	{
		int i;
		fprintf(pfile1, "%s", cp2);
		for(i = 0; i < 8192; i++)
		{
			cp2[i] = 0;
		}
	}
	fclose(pfile1);
	
	//send ack 2 that body 1 has been written
	sendMsg(fd, "Sender ACK2");
	
	//wait for header 2
	while(recv(fd, header2, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	//puts(header2);
	/*char cp3[8192];
	while(recv(fd, cp3, 8192, MSG_DONTWAIT) > 0)
	{
		int i;
		strcat(header2, cp3);
		for(i = 0; i < 8192; i++)
		{
			cp3[i] = 0;
		}
	}*/
	
	//grab filename 2
	if(strlen(header2) > 0)
	{
		filename2 = getFileName(header2);
		fileusr2 = getUserName(header2);
		if(strlen(filename2) > 0)
		{
			//send third ack that header 2 has been received
			sendMsg(fd, "Sender ACK 3");
		}
		else
		{
			filename2 = "tempfile.txt";
		}
	}
	//wait for body 2
	while(recv(fd, body2, 8192, MSG_DONTWAIT) <= 0)
	{
	} 
	//write body 2
	sprintf(filen2, "%s", filename2);
	sprintf(fileu2, "%s/", fileusr2);
	strcat(writefile2, fileu2);
	strcat(writefile2, filen2);
	pfile2 = fopen(writefile2, "w");
	fprintf(pfile2, "%s", body2);
	//receive request
	char cp4[8192];
	while(recv(fd, cp4, 8192, MSG_DONTWAIT) > 0)
	{
		int i;
		fprintf(pfile2, "%s", cp4);
		for(i = 0; i < 8192; i++)
		{
			cp4[i] = 0;
		}
	}
	//send final ack that body 2 has been written
	fclose(pfile2);
	sendMsg(fd, "Sender ACK 4");
}

void list(int fd, char *header)
{
	while(port == 8080)
	{
	}
	char allfiles[8192];
	char prefix[60][1024];
	char uniquePre[60][1024];
	char sendPre[8192];
	char *dir;
	char path[2048];
	if(getcwd(path, sizeof(path)) != NULL);
	char allACK[8192];
	DIR *dp;
	struct dirent *ep;
	//get all files in user directory
	dir = getUserName(header);
	//puts(dir);
	//sprintf(path, "/home/user/Dropbox/Networking_Systems/PA2/DFS1/%s/", dir);
	strcat(path, "/");
	strcat(path, DFS);
	strcat(path, "/");
	strcat(path, dir);
	//puts(path);
	int i = 0;
	dp = opendir(path);
	if (dp != NULL)
	{
		while ((ep = readdir(dp)))
		{
			if(i > 59) break;
			if(strlen(ep->d_name) > 2)
			{
				strcat(allfiles, ep->d_name);
				strcat(allfiles, ",");
				int count = 0;
				int index;
				for(index = 0; index < strlen(ep->d_name); index++)
				{
					if(ep->d_name[index] == '.') count++;
					if(count == 2) break;
				}
				strncpy(prefix[i], ep->d_name, index);
				//printf("File copied to buffer: %s\n", ep->d_name);
				i++;
			}
		}
		(void) closedir (dp);
	}
	else
	{
		perror ("Couldn't open the directory");
	}
	int j;
	int k;
	int u = 0;
	int isU;
	//check for unique prefixes
	for(j = 0; j < i; j++)
	{
		isU = 1;
		for(k = 0; k < u; k++)
		{
			if(strcmp(uniquePre[k], prefix[j]) == 0)
			{
				isU = 0;
			}
		}
		if(isU == 1)
		{
			strcpy(uniquePre[u],prefix[j]);
			u++;
		}
	}
	//make a buffer to send the unique prefixes
	for(j = 0; j < u; j++)
	{
		strcat(sendPre, uniquePre[j]);
		strcat(sendPre, ",");
	}
	//send all unique prefixes
	int bytesToSend = strlen(sendPre);
	ssize_t numBytesSent;
	do
	{
		numBytesSent = send(fd, sendPre, bytesToSend, MSG_NOSIGNAL);
		if(numBytesSent <= 0) break;
		bytesToSend -= numBytesSent;	
	}while(bytesToSend > 0);
	//wait for ack then send all files
	while(recv(fd, allACK, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	//send all files after reading ack
	bytesToSend = strlen(allfiles);
	do
	{
		numBytesSent = send(fd, allfiles, bytesToSend, MSG_NOSIGNAL);
		if(numBytesSent <= 0) break;
		bytesToSend -= numBytesSent;	
	}while(bytesToSend > 0);
	
}

void get(int fd, char *header)
{
	char allfiles[8192];
	char sendFile[8192];
	char sendFile2[8192];
	char amount[8192];
	char *dir;
	char path[2048];
	if(getcwd(path, sizeof(path)) != NULL);
	unsigned char buff[8192];
	unsigned char buff1[8192];
	FILE *fp;
	DIR *dp;
	struct dirent *ep;
	//get all files in user directory
	dir = getUserName(header);
	//puts(dir);
	strcat(path, "/");
	strcat(path, DFS);
	strcat(path, "/");
	strcat(path, dir);
	dp = opendir(path);
	char *file = getFileName(header);
	if (dp != NULL)
	{
		while ((ep = readdir(dp)))
		{
			if(strlen(ep->d_name) > 2)
			{
				int count = 0;
				int index;
				for(index = 0; index < strlen(ep->d_name); index++)
				{
					if(ep->d_name[index] == '.') count++;
					if(count == 2) break;
				}
				//grab suffix
				if(strncmp(file, ep->d_name, index) == 0)
				{
					int length = strlen(allfiles);
					allfiles[length] = ep->d_name[strlen(ep->d_name) - 1];
					strcat(allfiles, ",");
				}
			}
		}
		(void) closedir (dp);
		//printf("Suffixes copied to buffer: %s\n", allfiles);
	}
	else
	{
		perror ("Couldn't open the directory");
	}
	//send suffixes to client
	int bytesToSendS = strlen(allfiles);
	ssize_t numBytesSent;
	do
	{
		numBytesSent = send(fd, allfiles, bytesToSendS, MSG_NOSIGNAL);
		if(numBytesSent <= 0) break;
		bytesToSendS -= numBytesSent;	
	}while(bytesToSendS > 0);
	
	while(recv(fd, amount, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	//puts(amount);
	int numberOfFiles;
	numberOfFiles = amount[strlen(amount) - 1] - '0';
	sendMsg(fd, "Amount ACK");
	//wait to hear from client as to which files it needs
	while(recv(fd, sendFile, 8192, MSG_DONTWAIT) <= 0)
	{
	}

	if(numberOfFiles == 0) return;
	//send all files the client requested
	//puts(sendFile);
	
	printf("numberOfFiles = %d\n", numberOfFiles);
	
	if(numberOfFiles > 0)
	{
		strcat(path, "/");
		strcat(path, sendFile);
		fp = fopen(path, "r");
		while(!feof(fp))
		{
			size_t bytesToSendF;
			bytesToSendF = fread(buff, sizeof(unsigned char), 1024, fp);
			unsigned char *line = buff;
			do
			{
				numBytesSent = send(fd, line, bytesToSendF, MSG_NOSIGNAL);
				//printf("01: Number of File Bytes Sent: %zd\n", numBytesSent);
				if(numBytesSent <= 0) break;
				line += numBytesSent;
				bytesToSendF -= numBytesSent;
			}while(bytesToSendF > 0);
		}
		fclose(fp);
		if(numberOfFiles == 2)
		{
			//puts("in 2nd file send");
			while(recv(fd, sendFile2, 8192, 0) < 0)
			{
			}
			char cp[8192];
			while(recv(fd, cp, 8192, MSG_DONTWAIT) >= 0)
			{
				int i;
				strcat(sendFile2, cp);
				for(i = 0; i < 8192; i++)
				{
					cp[i] = 0;
				}
			}
			if(getcwd(path, sizeof(path)) != NULL);
			strcat(path, "/");
			strcat(path, DFS);
			strcat(path, "/");
			strcat(path, dir);
			strcat(path, "/");
			strcat(path, sendFile2);
			fp = fopen(path, "r");
			while(!feof(fp))
			{
				size_t bytesToSendF;
				bytesToSendF = fread(buff1, sizeof(unsigned char), 1024, fp);
				unsigned char *line = buff1;
				do
				{
					numBytesSent = send(fd, line, bytesToSendF, MSG_NOSIGNAL);
				//	printf("02: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					bytesToSendF -= numBytesSent;
				}while(bytesToSendF > 0);
			}
			fclose(fp);
		}
	}
	
	
}

void con(void *args)
{
	//set up variables
	parseConf();
	struct clientSock* arguments = args;
	int* temp = arguments->sockets;
	int fd = *temp;
	char header[8192];
	char upCheck[8192];
	char *function;
	char *pass;
	char *user;
	//receive first header
	recv(fd, upCheck, 8192, 0);
	char cp[8192];
	while(recv(fd, cp, 8192, MSG_DONTWAIT) > 0)
	{
		int i;
		strcat(upCheck, cp);
		for(i = 0; i < 8192; i++)
		{
			cp[i] = 0;
		}
	}
	
	pass = getPassword(upCheck);
	user = getUserName(upCheck);
	int checkCred = check(user, pass);
	if(checkCred != 1)
	{
		sendMsg(fd, "Invalid Username/Password. Please try again.");
	}
	if(checkCred == 1)
	{
		sendMsg(fd, "Correct User and Password!");
		recv(fd, header, 8192, 0);
		char cp1[8192];
		while(recv(fd, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(header, cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		}
		//get function name and forward to correct handler
		if(strlen(header) > 0)
		{
			
			function = getFunction(header);
			if(strcmp(function, "PUT") == 0)
			{
				put(fd, header);
			}
			else if(strcmp(function, "LIST") == 0)
			{
				list(fd, header);
			}
			else if(strcmp(function, "GET") == 0)
			{
				get(fd, header);
			}
		}
	}
	close(fd);
	free(arguments);
	pthread_exit((void *)0);
}

int main(int argc, char *argv[])
{
	//parse ws.conf and set up variables
	port = atoi(argv[1]);
	DFS = argv[2];
	int sock;
	int listener;
	struct sockaddr_in cli_addr;
	struct sockaddr_in serv_addr;
	socklen_t cli_len = sizeof(cli_addr);
	//make server socket
	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		perror("Error on socket creation");
		exit(-1);
	}
	//zero adn set attributes of server struct
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	//bind the socket
	if(bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 )
	{
		perror("Error on bind");
		exit(-1);
	}
	//listen for connections on the server socket
	if(listen(sock, 5) == -1)
	{
		perror("Error on listen");
		exit(-1);
	}
	while(1)
	{
		//accept and make client socket
		struct clientSock *clisocket = malloc(sizeof(struct clientSock));
		listener = accept(sock, (struct sockaddr*)&cli_addr, &cli_len);
		clisocket->sockets = &listener;
		printf("got connection\n");
		if(listener == -1)
		{
			perror("Error on accept");
		}
		//allow for concurrent connections using pthreads
		pthread_t tid;
		int rc;
		rc = pthread_create(&tid, NULL,(void *) con, (void *) clisocket);
		if(rc)
		{
			perror("Error in pthread_create");
			close(sock);
			exit(EXIT_FAILURE);
		}
	}
	shutdown(sock, SHUT_RD);
	return 0;
}



