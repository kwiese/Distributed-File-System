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
#include <openssl/md5.h>
#include <arpa/inet.h>
#include <time.h>


#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char uname[1][1024];
char password[1][1024];
char ip[4][1024];
int port[4];


struct clientSock
{
	int *socket1;
	int *socket2;
	int *socket3;
	int *socket4;
};

int fileSize(int fd);
void sendMsg(int fd, char *msg);
int hash(char *file);
int md50Put(struct clientSock sockets, char *file, char *method);
int md51Put(struct clientSock sockets, char *file, char *method);
int md52Put(struct clientSock sockets, char *file, char *method);
int md53Put(struct clientSock sockets, char *file, char *method);
int get(struct clientSock sockets, char *file, char *method);
int list(struct clientSock sockets, char *method);
int check(struct clientSock sockets, char *method);
char **getTypes(char stringToSplit[]);
char **getTypes2(char stringToSplit[]);
void parseConf();

int main(int argc, char *argv[])
{
	//initialize variables
	int md5;
	int sock1, sock2, sock3, sock4;
	struct sockaddr_in server;
	char *file;
	parseConf();
	if(argc > 2)
	{
		file = argv[2];
		md5 = hash(file);
		//md5 = 0;
	}
	
	char *method = argv[1];
	puts(method);
	
	//make connections with servers
	sock1 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock1 < 0)
	{
	  printf("could not create socket");
	}
	puts("Socekt1 created");

	server.sin_addr.s_addr = inet_addr(ip[0]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port[0]);

	if(connect(sock1, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected 1");
	
	sock2 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock2 < 0)
	{
	  printf("could not create socket");
	}
	puts("Socekt2 created");

	server.sin_addr.s_addr = inet_addr(ip[0]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port[1]);

	if(connect(sock2, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected 2");
	
	sock3 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock3 < 0)
	{
	  printf("could not create socket");
	}
	puts("Socekt3 created");

	server.sin_addr.s_addr = inet_addr(ip[0]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port[2]);

	if(connect(sock3, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected3");
	
	sock4 = socket(AF_INET, SOCK_STREAM, 0);
	if (sock4 < 0)
	{
	  printf("could not create socket");
	}
	puts("Socekt4 created");

	server.sin_addr.s_addr = inet_addr(ip[0]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port[3]);

	if(connect(sock4, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	puts("Connected4");
	
	//place sockets in struct to pass into functions
	struct clientSock *clisocket = malloc(sizeof(struct clientSock));
	clisocket->socket1 = &sock1;
	clisocket->socket2 = &sock2;
	clisocket->socket3 = &sock3;
	clisocket->socket4 = &sock4;
	
	//send user name and password to servers to varify
	int checkUP = check(*clisocket, method);
	printf("%d\n", checkUP);
	//call functions to run desired method
	if(checkUP == 1)
	{
		if(strcmp(method, "LIST") == 0)
		{
			list(*clisocket, method);
		}
		else
		{
			if(md5 == 0)
			{
				if(strcmp(method, "PUT") == 0)
				{
					puts("in put!!");
					md50Put(*clisocket, file, method);
				}
				else if(strcmp(method, "GET") == 0)
				{
					puts("in get call");
					get(*clisocket, file, method);
				}
			}
			else if(md5 == 1)
			{
				if(strcmp(method, "PUT") == 0)
				{
					md51Put(*clisocket, file, method);
				}
				else if(strcmp(method, "GET") == 0)
				{
					get(*clisocket, file, method);
				}
			}
			else if(md5 == 2)
			{
				if(strcmp(method, "PUT") == 0)
				{
					md52Put(*clisocket, file, method);
				}
				else if(strcmp(method, "GET") == 0)
				{
					get(*clisocket, file, method);
				}
			}
			else if(md5 == 3)
			{
				if(strcmp(method, "PUT") == 0)
				{
					md53Put(*clisocket, file, method);
				}
				else if(strcmp(method, "GET") == 0)
				{
					get(*clisocket, file, method);
				}
			}
		}
	}
	printf("Done with function: %s!\n", method);
	shutdown(sock1, SHUT_RD);
	shutdown(sock2, SHUT_RD);
	shutdown(sock3, SHUT_RD);
	shutdown(sock4, SHUT_RD);
	return 0;
}

int check(struct clientSock sockets, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	char ack1[2048];
	char ack2[2048];
	char ack3[2048];
	char ack4[2048];

	sprintf(header1, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header2, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header3, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header4, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	
	//semd usernames and passwords to servers to be varified
	sendMsg(num1, header1);
	while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
	{
	}

	sendMsg(num2, header2);
	while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	

	sendMsg(num3, header3);
	while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	

	sendMsg(num4, header4);
	while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
	{
	}
	printf("DFS1: %s\n", ack1);
	printf("DFS2: %s\n", ack2);
	printf("DFS3: %s\n", ack3);
	printf("DFS4: %s\n", ack4);
	if(strcmp(ack1, "Invalid Username/Password. Please try again.") == 0)
	{
		return -1;
	}
	if(strcmp(ack2, "Invalid Username/Password. Please try again.") == 0)
	{
		return -1;
	}
	if(strcmp(ack3, "Invalid Username/Password. Please try again.") == 0)
	{
		return -1;
	}
	if(strcmp(ack4, "Invalid Username/Password. Please try again.") == 0)
	{
		return -1;
	}
	return 1;
}

int hash(char *file)
{
	//make an md5 hash of the file
	unsigned char c[MD5_DIGEST_LENGTH];
    char *filename="sampletxt.txt";
    FILE *inFile = fopen (filename, "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
	int res = 0;
    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename);
        return 0;
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    printf("\nRes: %d\n", c[MD5_DIGEST_LENGTH - 1]);
    res = c[MD5_DIGEST_LENGTH - 1];
    res = res % 4;
    printf ("%d\n", res);
    fclose (inFile);
    return res;
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
		//printf("Header:\n%s\n", msg);
		numBytesSent = send(fd, msg, size, MSG_NOSIGNAL);
		//printf("Bytes Sent: %d out of %d\n", numBytesSent, size);
		if(numBytesSent <= 0) break;
		size -= numBytesSent;
		msg += numBytesSent;
	}while(size > 0);
}

char **getTypes(char stringToSplit[])
{
	//splits up string based on spaces
	char **type = malloc(2*sizeof *type);
	const char d[] = ":";
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
	//splits up string based on commas
	char **type = malloc(60*sizeof *type);
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

void parseConf()
{
	//parse dfc.conf file
	char findDFS[] = "#DFS's\n";
	char findUser[] = "#User Info\n";
	char *lis[2];
	lis[0] = strdup(findDFS);
	lis[1] = strdup(findUser);
	
	int dInd, uInd;
	int i = 0;
	int j = 0;
	
	int numLines = 0;
	 char line[256];
	 char* lines[20];
	 char *pos;
	 char **types;
	 
	 FILE *fp = fopen("dfc.conf", "r+");
	 if(fp == NULL)
	 {
		 printf("error");
		 exit(-1);
	 }
	
	 while(fgets(line, sizeof(line), fp) != NULL)
	 {
		lines[j] = strdup(line);
		j++;
		numLines++;
	 }
	 fclose(fp);
	
	 for(i = 0; i < numLines; i++)
	 {
		 if(strcmp(lines[i],lis[0]) == 0)
		 {
			 dInd = i + 1;
		 }
		 if(strcmp(lines[i],lis[1]) == 0)
		 {
			 uInd = i + 1;
		 }
	 }
	 j = 0;
	 for(i = dInd; i < uInd - 1; i++)
	 {
		 types = getTypes(lines[i]);
		 strcpy(ip[j], types[0]);
		 port[j] = atoi(types[1]);
		 j++;
	 }
	 
	 types = getTypes(lines[uInd]);
	 strcpy(uname[0], types[1]);
	 types = getTypes(lines[uInd + 1]);
	 strcpy(password[0], types[1]);
	 if((pos = strchr(uname[0], '\n')) != NULL)
	 {
		 *pos = '\0';
	 }
	 if((pos = strchr(password[0], '\n')) != NULL)
	 {
		 *pos = '\0';
	 }
}

int list(struct clientSock sockets, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	char tempPre[8192];
	char tempPre2[8192];
	char tempPre3[8192];
	char tempPre4[8192];
	char tempNames[8192];
	char tempNames2[8192];
	char tempNames3[8192];
	char tempNames4[8192];
	char cp1[8192];
	char cp2[8192];
	char cp3[8192];
	char cp4[8192];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	
	int i;
	//header that includes the users and passwords
	sprintf(header1, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header2, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header3, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	sprintf(header4, ",User: %s,Password: %s,Function: %s", uname[0], password[0], method);
	//send first header, wait and write prefixes of all files
	sendMsg(num1, header1);
	sentHeader = time(NULL);
	while(recv(num1, tempPre, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	if(timeout1 != 1)
	{
		while(recv(num1, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempPre, cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		}
	}
	
	//send second header, wait and write prefixes of all files
	sendMsg(num2, header2);
	sentHeader = time(NULL);
	while(recv(num2, tempPre2, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	if(timeout2 != 1)
	{
		while(recv(num2, cp2, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempPre2, cp2);
			for(i = 0; i < 8192; i++)
			{
				cp2[i] = 0;
			}
		}
	}
	
	//send third header, wait and write prefixes of all files
	sendMsg(num3, header3);
	sentHeader = time(NULL);
	while(recv(num3, tempPre3, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
	if(timeout3 != 1)
	{
		while(recv(num3, cp3, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempPre3, cp3);
			for(i = 0; i < 8192; i++)
			{
				cp3[i] = 0;
			}
		}
	}
	
	//send last header, wait and write prefixes of all files
	sendMsg(num4, header4);
	sentHeader = time(NULL);
	while(recv(num4, tempPre4, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	if(timeout4 != 1)
	{
		while(recv(num4, cp4, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempPre4, cp4);
			for(i = 0; i < 8192; i++)
			{
				cp4[i] = 0;
			}
		}
	}
	//combine all prefixes
	strcat(tempPre, tempPre2);
	strcat(tempPre, tempPre3);
	strcat(tempPre, tempPre4);
	
	//send acks that client recieved all prefixes and wait for all file names
	sendMsg(num1, "Client ACK 1");
	sentHeader = time(NULL);
	while(recv(num1, tempNames, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	if(timeout1 != 1)
	{
		while(recv(num1, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempNames, cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		} 
	}
	
	
	sendMsg(num2, "Client ACK 1");
	sentHeader = time(NULL);
	while(recv(num2, tempNames2, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	if(timeout2 != 1)
	{
		while(recv(num2, cp2, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempNames2, cp2);
			for(i = 0; i < 8192; i++)
			{
				cp2[i] = 0;
			}
		} 
	}
	
	
	sendMsg(num3, "Client ACK 1");
	sentHeader = time(NULL);
	while(recv(num3, tempNames3, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
	if(timeout3 != 1)
	{
		while(recv(num3, cp3, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempNames3, cp3);
			for(i = 0; i < 8192; i++)
			{
				cp3[i] = 0;
			}
		}
	}
	
	sendMsg(num4, "Client ACK 1");
	sentHeader = time(NULL);
	while(recv(num4, tempNames4, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	if(timeout4 != 1)
	{
		while(recv(num4, cp4, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempNames4, cp4);
			for(i = 0; i < 8192; i++)
			{
				cp4[i] = 0;
			}
		} 
	}
	//combine responses
	strcat(tempNames, tempNames2);
	strcat(tempNames, tempNames3);
	strcat(tempNames, tempNames4);
	
	//split up responses into easily accessible array
	int numPre = 0;
	for(i = 0; i < strlen(tempPre); i++)
	{
		if(tempPre[i] == ',')
		{
			numPre++;
		}
	}
	numPre--;
	int numfullNames = 0;
	for(i = 0; i < strlen(tempNames); i++)
	{
		if(tempNames[i] == ',')
		{
			numfullNames++;
		}
	}
	numfullNames--;
	//printf("Pre: %d\n", numPre);
	//printf("Names: %d\n", numfullNames);
	char **prefixes = getTypes2(tempPre);
	char **fullNames = getTypes2(tempNames);
	//printf("%zd\n", strlen(tempPre));
	
	char uniquePre[60][1024];
	int j;
	int k;
	int numUnique = 0;
	int isU = 1;
	
	//check for unique prefixes
	//puts("Printing Unique Prefixes");
	for(j = 0; j < numPre; j++)
	{
		isU = 1;
		for(k = 0; k < numUnique; k++)
		{
			if(strcmp(uniquePre[k], prefixes[j]) == 0)
			{
				isU = 0;
				break;
			}
		}
		if(isU == 1)
		{
			strcpy(uniquePre[numUnique], prefixes[j]);
			//puts(uniquePre[numUnique]);
			numUnique++;
		}
	}
	//initialize file check part 1
	int fileCheck[numUnique][4];
	for(i = 0; i < numUnique; i++)
	{
		for(j = 0; j < 4; j++)
		{
			fileCheck[numUnique][j] = 0;
		}
	}
	
	//use array to check to see if all parts of a file are present
	for(i = 0; i < numUnique; i++)
	{
		for(j = 0; j < numfullNames; j++)
		{
			if((strlen(uniquePre[i]) + 2) == strlen(fullNames[j]))
			{
				if(strncmp(uniquePre[i], fullNames[j], strlen(uniquePre[i])) == 0)
				{
					char num = fullNames[j][strlen(fullNames[j]) - 1];
					int filePart = num - '0';
					//printf("File part: %d\n", filePart); 
					fileCheck[i][filePart - 1] = 1;
				}
			}
		}
	}
	//puts("Printing fileCheck vals:");
	for(i = 0; i < numUnique; i++)
	{
		for(j = 0; j < 4; j++)
		{
			//printf("[%d][%d]: %d\n", i, j, fileCheck[i][j]); 
		}
	}
	
	//initialize filecheck part 2
	int isOkay[numUnique];
	//puts("Printing isOkay vals:");
	for(i = 0; i < numUnique; i++)
	{
		isOkay[i] = 1;
		//printf("%d: %d\n", i, isOkay[i]);
	}
	//check to see if all parts are present
	for(i = 0; i < numUnique; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(fileCheck[i][j] == 0)
			{
				isOkay[i] = 0;
				break;
			}
		}
	}
	//print response
	puts("Printing LIST response:");
	for(i = 0; i < numUnique; i++)
	{
		if(isOkay[i] == 0)
		{
			printf("%s [incomplete]\n", uniquePre[i]);
		}
		else
		{
			printf("%s\n", uniquePre[i]);
		}
	}
	
	return 1;
}

int md50Put(struct clientSock sockets, char *file, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	int k, resourcefd, length;
	FILE *fp1;
	FILE *fp2;
	unsigned char buff[1024];
	unsigned char buff2[1024];
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	//printf("Ports: %d,%d,%d,%d\n", port[0], port[1], port[2], port[3]);
	sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	
	resourcefd = open(file, O_RDONLY, 0);
	length = fileSize(resourcefd);
	close(resourcefd);
	k = length / 4;

	//send headers to servers indicating which file is going to be put on the server
	sendMsg(num1, header1);
	sentHeader = time(NULL);
	char ack01[8192];
	while(recv(num1, ack01, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	sendMsg(num2, header2);
	sentHeader = time(NULL);
	char ack02[8192];
	while(recv(num2, ack02, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	sendMsg(num3, header3);
	sentHeader = time(NULL);
	char ack03[8192];
	while(recv(num3, ack03, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
		
	sendMsg(num4, header4);
	sentHeader = time(NULL);
	char ack04[8192];
	while(recv(num4, ack04, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	//semd the first file to the correct servers
	fp1 = fopen(file, "r");
	int total = 0;
	while(!feof(fp1))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff, sizeof(unsigned char), k, fp1);
		unsigned char *line = buff;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout1 == 0)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					printf("01: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout2 == 0)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					printf("02: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout3 == 0)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					printf("03: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout4 == 0)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					printf("04: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp1);
	
	if(timeout1 != 1)
	{
		char ack001[8192];
		while(recv(num1, ack001, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout2 != 1)
	{
		char ack002[8192];
		while(recv(num2, ack002, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout3 != 1)
	{
		char ack003[8192];
		while(recv(num3, ack003, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout4 != 1)
	{
		char ack004[8192];
		while(recv(num4, ack004, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	
	
	//semd the names of the second file to be placed on each server
	if(timeout1 != 1)
	{
		sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num1, header2);
		sentHeader = time(NULL);
		char ack1[8192];
		while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout1 = 1;
				break;
			}
		}
	}
	
	
	if(timeout2 != 1)
	{
		sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num2, header3);
		sentHeader = time(NULL);
		char ack2[8192];
		while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout2 = 1;
				break;
			}
		}
	}
	
	
	if(timeout3 != 1)
	{
		sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num3, header4);
		sentHeader = time(NULL);
		char ack3[8192];
		while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout3 = 1;
				break;
			}
		}
	}
	
	if(timeout4 != 1)
	{
		sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num4, header1);
		sentHeader = time(NULL);
		char ack4[8192];
		while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout4 = 1;
				break;
			}
		}
	}
	
	//send the correct parts of the second file to the correct servers
	fp2 = fopen(file, "r");
	total = 0;
	while(!feof(fp2))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff2, sizeof(unsigned char), k, fp2);
		unsigned char *line = buff2;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout4 != 1)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("1: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout1 != 1)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("2: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout2 != 1)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("3: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout3 != 1)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("4: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp2);

	return 1;
}

int md51Put(struct clientSock sockets, char *file, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	int k, resourcefd, length;
	FILE *fp1;
	FILE *fp2;
	unsigned char buff[1024];
	unsigned char buff2[1024];
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	//printf("Ports: %d,%d,%d,%d\n", port[0], port[1], port[2], port[3]);
	sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	
	resourcefd = open(file, O_RDONLY, 0);
	length = fileSize(resourcefd);
	close(resourcefd);
	k = length / 4;
	
	//send the file name of the first file each server is going to receive
	sendMsg(num1, header4);
	sentHeader = time(NULL);
	char ack01[8192];
	while(recv(num1, ack01, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	sendMsg(num2, header1);
	sentHeader = time(NULL);
	char ack02[8192];
	while(recv(num2, ack02, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	sendMsg(num3, header2);
	sentHeader = time(NULL);
	char ack03[8192];
	while(recv(num3, ack03, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
		
	sendMsg(num4, header3);
	sentHeader = time(NULL);
	char ack04[8192];
	while(recv(num4, ack04, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}

	//send the correct parts of file 1 to the correct servers
	fp1 = fopen(file, "r");
	int total = 0;
	while(!feof(fp1))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff, sizeof(unsigned char), k, fp1);
		unsigned char *line = buff;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout1 == 0)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("01: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout2 == 0)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("02: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout3 == 0)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("03: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout4 == 0)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("04: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp1);
	
	if(timeout1 != 1)
	{
		char ack001[8192];
		while(recv(num1, ack001, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout2 != 1)
	{
		char ack002[8192];
		while(recv(num2, ack002, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout3 != 1)
	{
		char ack003[8192];
		while(recv(num3, ack003, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout4 != 1)
	{
		char ack004[8192];
		while(recv(num4, ack004, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	
	
	//send the file parts of the second file each server will receive
	if(timeout1 != 1)
	{
		sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num1, header1);
		sentHeader = time(NULL);
		char ack1[8192];
		while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout1 = 1;
				break;
			}
		}
	}
	
	
	if(timeout2 != 1)
	{
		sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num2, header3);
		sentHeader = time(NULL);
		char ack2[8192];
		while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout2 = 1;
				break;
			}
		}
	}
	
	
	if(timeout3 != 1)
	{
		sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num3, header3);
		sentHeader = time(NULL);
		char ack3[8192];
		while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout3 = 1;
				break;
			}
		}
	}
	
	if(timeout4 != 1)
	{
		sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num4, header4);
		sentHeader = time(NULL);
		char ack4[8192];
		while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout4 = 1;
				break;
			}
		}
	}
	
	//send the correct second parts of the file to the correct server
	fp2 = fopen(file, "r");
	total = 0;
	while(!feof(fp2))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff2, sizeof(unsigned char), k, fp2);
		unsigned char *line = buff2;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout4 != 1)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("1: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout1 != 1)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("2: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout2 != 1)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("3: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout3 != 1)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("4: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp2);

	return 1;
}

int md52Put(struct clientSock sockets, char *file, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	int k, resourcefd, length;
	FILE *fp1;
	FILE *fp2;
	unsigned char buff[1024];
	unsigned char buff2[1024];
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	//printf("Ports: %d,%d,%d,%d\n", port[0], port[1], port[2], port[3]);
	sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	
	resourcefd = open(file, O_RDONLY, 0);
	length = fileSize(resourcefd);
	close(resourcefd);
	k = length / 4;

	//send the first parts each server will receive
	sendMsg(num1, header3);
	sentHeader = time(NULL);
	char ack01[8192];
	while(recv(num1, ack01, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	sendMsg(num2, header4);
	sentHeader = time(NULL);
	char ack02[8192];
	while(recv(num2, ack02, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	sendMsg(num3, header1);
	sentHeader = time(NULL);
	char ack03[8192];
	while(recv(num3, ack03, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
		
	sendMsg(num4, header2);
	sentHeader = time(NULL);
	char ack04[8192];
	while(recv(num4, ack04, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}

	//send the correct first parts of each file to the correct servers
	fp1 = fopen(file, "r");
	int total = 0;
	while(!feof(fp1))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff, sizeof(unsigned char), k, fp1);
		unsigned char *line = buff;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout1 == 0)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("01: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout2 == 0)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("02: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout3 == 0)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("03: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout4 == 0)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("04: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp1);
	
	if(timeout1 != 1)
	{
		char ack001[8192];
		while(recv(num1, ack001, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout2 != 1)
	{
		char ack002[8192];
		while(recv(num2, ack002, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout3 != 1)
	{
		char ack003[8192];
		while(recv(num3, ack003, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout4 != 1)
	{
		char ack004[8192];
		while(recv(num4, ack004, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	
	
	//send the names of the second file parts that each file will receive
	if(timeout1 != 1)
	{
		sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num1, header4);
		sentHeader = time(NULL);
		char ack1[8192];
		while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout1 = 1;
				break;
			}
		}
	}
	
	
	if(timeout2 != 1)
	{
		sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num2, header1);
		sentHeader = time(NULL);
		char ack2[8192];
		while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout2 = 1;
				break;
			}
		}
	}
	
	
	if(timeout3 != 1)
	{
		sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num3, header2);
		sentHeader = time(NULL);
		char ack3[8192];
		while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout3 = 1;
				break;
			}
		}
	}
	
	if(timeout4 != 1)
	{
		sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num4, header3);
		sentHeader = time(NULL);
		char ack4[8192];
		while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout4 = 1;
				break;
			}
		}
	}
	
	//send the correct second file parts to the correct servers
	fp2 = fopen(file, "r");
	total = 0;
	while(!feof(fp2))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff2, sizeof(unsigned char), k, fp2);
		unsigned char *line = buff2;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout4 != 1)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("1: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout1 != 1)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("2: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout2 != 1)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("3: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout3 != 1)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("4: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp2);

	return 1;
}

int md53Put(struct clientSock sockets, char *file, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	int k, resourcefd, length;
	FILE *fp1;
	FILE *fp2;
	unsigned char buff[1024];
	unsigned char buff2[1024];
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	//printf("Ports: %d,%d,%d,%d\n", port[0], port[1], port[2], port[3]);
	sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	
	resourcefd = open(file, O_RDONLY, 0);
	length = fileSize(resourcefd);
	close(resourcefd);
	k = length / 4;

	//send the correct first file part names to the correct servers
	sendMsg(num1, header2);
	sentHeader = time(NULL);
	char ack01[8192];
	while(recv(num1, ack01, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	sendMsg(num2, header3);
	sentHeader = time(NULL);
	char ack02[8192];
	while(recv(num2, ack02, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	sendMsg(num3, header4);
	sentHeader = time(NULL);
	char ack03[8192];
	while(recv(num3, ack03, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
		
	sendMsg(num4, header1);
	sentHeader = time(NULL);
	char ack04[8192];
	while(recv(num4, ack04, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}

	//send the file parts to the correct server
	fp1 = fopen(file, "r");
	int total = 0;
	while(!feof(fp1))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff, sizeof(unsigned char), k, fp1);
		unsigned char *line = buff;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout1 == 0)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("01: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout2 == 0)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("02: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout3 == 0)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("03: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout4 == 0)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("04: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp1);
	
	if(timeout1 != 1)
	{
		char ack001[8192];
		while(recv(num1, ack001, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout2 != 1)
	{
		char ack002[8192];
		while(recv(num2, ack002, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout3 != 1)
	{
		char ack003[8192];
		while(recv(num3, ack003, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	if(timeout4 != 1)
	{
		char ack004[8192];
		while(recv(num4, ack004, 8192, MSG_DONTWAIT) <= 0)
		{
		}
	}
	
	
	
	//send the second wave of file parts to the correct servers
	if(timeout1 != 1)
	{
		sprintf(header3, "File: %s.3,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num1, header3);
		sentHeader = time(NULL);
		char ack1[8192];
		while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout1 = 1;
				break;
			}
		}
	}
	
	
	if(timeout2 != 1)
	{
		sprintf(header4, "File: %s.4,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num2, header4);
		sentHeader = time(NULL);
		char ack2[8192];
		while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout2 = 1;
				break;
			}
		}
	}
	
	
	if(timeout3 != 1)
	{
		sprintf(header1, "File: %s.1,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num3, header1);
		sentHeader = time(NULL);
		char ack3[8192];
		while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout3 = 1;
				break;
			}
		}
	}
	
	if(timeout4 != 1)
	{
		sprintf(header2, "File: %s.2,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
		sendMsg(num4, header2);
		sentHeader = time(NULL);
		char ack4[8192];
		while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
		{
			if((((long long) (time(NULL) - sentHeader)) > 1))
			{
				timeout4 = 1;
				break;
			}
		}
	}
	
	//send the correct second wave of file parts to each server
	fp2 = fopen(file, "r");
	total = 0;
	while(!feof(fp2))
	{
		size_t bytesToSend;
		bytesToSend = fread(buff2, sizeof(unsigned char), k, fp2);
		unsigned char *line = buff2;
		ssize_t numBytesSent;
		do
		{
			if(total < k)
			{
				if(timeout4 != 1)
				{
					numBytesSent = send(num3, line, bytesToSend, MSG_NOSIGNAL);
					//printf("1: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (2 * k) && total >= k)
			{
				if(timeout1 != 1)
				{
					numBytesSent = send(num4, line, bytesToSend, MSG_NOSIGNAL);
					//printf("2: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < (3 * k) && total >= (2 *k))
			{
				if(timeout2 != 1)
				{
					numBytesSent = send(num1, line, bytesToSend, MSG_NOSIGNAL);
					//printf("3: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
				
			}
			else if(total < length - 1 && total >= (3 *k))
			{
				if(timeout3 != 1)
				{
					numBytesSent = send(num2, line, bytesToSend, MSG_NOSIGNAL);
					//printf("4: Number of File Bytes Sent: %zd\n", numBytesSent);
					if(numBytesSent <= 0) break;
					line += numBytesSent;
					total += numBytesSent;
					bytesToSend -= numBytesSent;
				}
				else
				{
					total += k;
					line += k;
					bytesToSend = 0;
				}
			}
			else
			{
				break;
			}
		}while(bytesToSend > 0);
	}
	fclose(fp2);
	puts("made it to end");
	return 1;
}

int get(struct clientSock sockets, char *file, char *method)
{
	int num1 = *sockets.socket1;
	int num2 = *sockets.socket2;
	int num3 = *sockets.socket3;
	int num4 = *sockets.socket4;
	char header1[2048];
	char header2[2048];
	char header3[2048];
	char header4[2048];
	char tempSuf[8192];
	char tempSuf1[8192];
	char tempSuf2[8192];
	char tempSuf3[8192];
	char body1[8192];
	char body2[8192];
	char body3[8192];
	char body4[8192];
	char ack1[2048];
	char ack2[2048];
	char ack3[2048];
	char ack4[2048];
	char cp1[8192];
	char cp2[8192];
	char cp3[8192];
	char cp4[8192];
	time_t sentHeader = 0;
	int timeout1 = 0;
	int timeout2 = 0;
	int timeout3 = 0;
	int timeout4 = 0;
	int all[4][4] = {
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	};
	int toSend[4][4] = {
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	};
	int DFSCheck[4] = {0,0,0,0};
	int fileParts[4] = {0,0,0,0};
	FILE *fp;
	
	sprintf(header1, "File: %s,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header2, "File: %s,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header3, "File: %s,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	sprintf(header4, "File: %s,User: %s,Password: %s,Function: %s", file, uname[0], password[0], method);
	
	//send file name to each server and wait for the numbers of files that each server has
	sendMsg(num1, header1);
	sentHeader = time(NULL);
	while(recv(num1, tempSuf, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	if(timeout1 != 1)
	{
		while(recv(num1, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempSuf, cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		}
	}

	sendMsg(num2, header2);
	sentHeader = time(NULL);
	while(recv(num2, tempSuf1, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	if(timeout2 != 1)
	{
		while(recv(num2, cp2, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempSuf1, cp2);
			for(i = 0; i < 8192; i++)
			{
				cp2[i] = 0;
			}
		}
	}

	sendMsg(num3, header3);
	sentHeader = time(NULL);
	while(recv(num3, tempSuf2, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
	if(timeout3 != 1)
	{
		while(recv(num3, cp3, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempSuf2, cp3);
			for(i = 0; i < 8192; i++)
			{
				cp3[i] = 0;
			}
		}
	}
	

	sendMsg(num4, header4);
	sentHeader = time(NULL);
	while(recv(num4, tempSuf3, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	if(timeout4 != 1)
	{
		while(recv(num4, cp4, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(tempSuf3, cp4);
			for(i = 0; i < 8192; i++)
			{
				cp4[i] = 0;
			}
		}
	}

	//deal with fact that response is all of the suffixes
	//populate matrix to keep track of which machine sent which suffix
	int i = 0;
	if(timeout1 != 1)
	{
		char **suffixes = getTypes2(tempSuf);
		while(suffixes[i] != NULL)
		{
			int part = atoi(suffixes[i]);
			all[0][part-1] = 1;
			i++;
		}
	}
	if(timeout2 != 1)
	{
		char **suffixes1 = getTypes2(tempSuf1);
		i = 0;
		while(suffixes1[i] != NULL)
		{
			int part = atoi(suffixes1[i]);
			all[1][part-1] = 1;
			i++;
		}
	}
	if(timeout3 != 1)
	{
		char **suffixes2 = getTypes2(tempSuf2);
		i = 0;
		while(suffixes2[i] != NULL)
		{
			int part = atoi(suffixes2[i]);
			all[2][part-1] = 1;
			i++;
		}
	}
	if(timeout4 != 1)
	{
		char **suffixes3 = getTypes2(tempSuf3);
		i = 0;
		while(suffixes3[i] != NULL)
		{
			int part = atoi(suffixes3[i]);
			all[3][part-1] = 1;
			i++;
		}
	}
	
	int k;
	for(i = 0; i < 4; i++)
	{
		for(k = 0; k < 4; k++)
		{
			printf("%d ", all[i][k]);
		}
		puts("");
	} 
	//populate new matrix to determine which parts to grab from where
	for(i = 0; i < 4; i++)
	{
		for(k = 0; k < 4; k++)
		{
			if(all[i][k] == 1 && fileParts[k] == 0)
			{
				fileParts[k] = 1;
				toSend[i][k] = 1;
			}
		}
	}
	for(i = 0; i < 4; i++)
	{
		for(k = 0; k < 4; k++)
		{
			printf("%d ", toSend[i][k]);
		}
		puts("");
	} 
	//check to see if all file parts are available
	for(i = 0; i < 4; i++)
	{
		if(fileParts[i] == 0)
		{
			puts("File is incomplete");
			return 0;
		}
	}
	//check to see which dfs we need to request from
	for(i = 0; i < 4; i++)
	{
		for(k = 0; k < 4; k++)
		{
			if(toSend[i][k] == 1)
			{
				DFSCheck[i] = 1;
			}
		}
	}
	//craft request string with which files we need to get from each dfs
	int numberOfFiles[4];
	int first, second, third, fourth;
	for(i = 0; i < 4; i++)
	{
		for(k = 0; k < 4; k++)
		{
			if(DFSCheck[i] == 1 && toSend[i][k] == 1)
			{
				numberOfFiles[i]++;
				if(k + 1 == 1)
					first = i + 1;
				else if(k + 1 == 2)
					second = i + 1;
				else if(k + 1 == 3)
					third = i + 1;
				else if(k + 1 == 4)
					fourth = i + 1;
			}
		}
	}
	//printf("FSTF: %d, %d, %d, %d\n", first, second, third, fourth);
	
	char amount1[128];
	char amount2[128];
	char amount3[128];
	char amount4[128];
	sprintf(amount1, "Amount: %d", numberOfFiles[0]);
	sprintf(amount2, "Amount: %d", numberOfFiles[1]);
	sprintf(amount3, "Amount: %d", numberOfFiles[2]);
	sprintf(amount4, "Amount: %d", numberOfFiles[3]);
	
	//send the amount of file parts needed from each server
	sendMsg(num1, amount1);
	sentHeader = time(NULL);
	while(recv(num1, ack1, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	if(timeout1 != 1)
	{
		while(recv(num1, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(ack1, cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		}
	}

	sendMsg(num2, amount2);
	sentHeader = time(NULL);
	while(recv(num2, ack2, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	if(timeout2 != 1)
	{
		while(recv(num2, cp2, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(ack2, cp2);
			for(i = 0; i < 8192; i++)
			{
				cp2[i] = 0;
			}
		}
	}

	sendMsg(num3, amount3);
	sentHeader = time(NULL);
	while(recv(num3, ack3, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
	if(timeout3 != 1)
	{
		while(recv(num3, cp3, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(ack3, cp3);
			for(i = 0; i < 8192; i++)
			{
				cp3[i] = 0;
			}
		}
	}
	

	sendMsg(num4, amount4);
	sentHeader = time(NULL);
	while(recv(num4, ack4, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	if(timeout4 != 1)
	{
		while(recv(num4, cp4, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			strcat(ack4, cp4);
			for(i = 0; i < 8192; i++)
			{
				cp4[i] = 0;
			}
		}
	}
	
	//send new header with file part it wants
	sprintf(header1, "%s.1", file);
	sprintf(header2, "%s.2", file);
	sprintf(header3, "%s.3", file);
	sprintf(header4, "%s.4", file);
	
	//printf("Sockets: %d, %d, %d, %d\n", *sockets.socket1, *sockets.socket2, *sockets.socket3, *sockets.socket4);
	//printf("num original: %d, %d, %d, %d\n", num1, num2, num3, num4);
	
	//assign correct first socket
	if(first == 1)
	{
		num1 = *sockets.socket1;
	}
	else if(first == 2)
	{
		num1 = *sockets.socket2;
	}
	else if(first == 3)
	{
		num1 = *sockets.socket3;
	}
	else if(first == 4)
	{
		num1 = *sockets.socket4;
	}
	
	//assign correct second socket
	if(second == 1)
	{
		num2 = *sockets.socket1;
	}
	else if(second == 2)
	{
		num2 = *sockets.socket2;
	}
	else if(second == 3)
	{
		num2 = *sockets.socket3;
	}
	else if(second == 3)
	{
		num2 = *sockets.socket4;
	}
	
	//assign correct third socket
	if(third == 1)
	{
		num3 = *sockets.socket1;
	}
	else if(third == 2)
	{
		num3 = *sockets.socket2;
	}
	else if(third == 3)
	{
		num3 = *sockets.socket3;
	}
	else if(third == 4)
	{
		num3 = *sockets.socket4;
	}
	
	//assign correct fourth socket
	if(fourth == 1)
	{
		num4 = *sockets.socket1;
	}
	else if(fourth == 2)
	{
		num4 = *sockets.socket2;
	}
	else if(fourth == 3)
	{
		num4 = *sockets.socket3;
	}
	else if(fourth == 4)
	{
		num4 = *sockets.socket4;
	}
	//printf("num new: %d, %d, %d, %d\n", num1, num2, num3, num4);
	//Send file requests and receive into file
	timeout1 = 0;
	timeout2 = 0;
	timeout3 = 0;
	timeout4 = 0;
	sendMsg(num1, header1);
	sentHeader = time(NULL);
	while(recv(num1, body1, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout1 = 1;
			break;
		}
	}
	if(timeout1 != 1)
	{
		fp = fopen(file, "a");
		fprintf(fp, "%s", body1);

		while(recv(num1, cp1, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			fprintf(fp, "%s", cp1);
			for(i = 0; i < 8192; i++)
			{
				cp1[i] = 0;
			}
		}
		fclose(fp);
	}


	sendMsg(num2, header2);
	sentHeader = time(NULL);
	while(recv(num2, body2, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout2 = 1;
			break;
		}
	}
	if(timeout2 != 1)
	{
		fp = fopen(file, "a");
		fprintf(fp, "%s", body2);

		while(recv(num2, cp2, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			fprintf(fp, "%s", cp2);
			for(i = 0; i < 8192; i++)
			{
				cp2[i] = 0;
			}
		}
		fclose(fp);
	}
	
	sendMsg(num3, header3);
	sentHeader = time(NULL);
	while(recv(num3, body3, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout3 = 1;
			break;
		}
	}
	if(timeout3 != 1)
	{
		fp = fopen(file, "a");
		fprintf(fp, "%s", body3);

		while(recv(num3, cp3, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			fprintf(fp, "%s", cp3);
			for(i = 0; i < 8192; i++)
			{
				cp3[i] = 0;
			}
		}
		fclose(fp);
	}
	
	sendMsg(num4, header4);
	sentHeader = time(NULL);
	while(recv(num4, body4, 8192, MSG_DONTWAIT) <= 0)
	{
		if((((long long) (time(NULL) - sentHeader)) > 1))
		{
			timeout4 = 1;
			break;
		}
	}
	if(timeout4 != 1)
	{
		fp = fopen(file, "a");
		fprintf(fp, "%s", body4);

		while(recv(num4, cp4, 8192, MSG_DONTWAIT) > 0)
		{
			int i;
			fprintf(fp, "%s", cp4);
			for(i = 0; i < 8192; i++)
			{
				cp4[i] = 0;
			}
		}
		fclose(fp);
	}
	
	return 1;
}
