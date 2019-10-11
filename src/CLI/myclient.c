#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "clientfunc.h"
//#include "clientfunc.c"

#define	SA	struct sockaddr
#define	MAXLINE		4096	/* max text line length */

typedef struct{
	char **IP;
	int *portnumbers;
	int number_of_servers;
}serverinfo;

typedef struct{
	int startB;
	int size;
	char *filename;
}threadinfo;

serverinfo *server_info;
//threadinfo *info;

int Filesize(char** argv);
void readserverinfo(char** argv);
void downloadfunction(char **argv, int FileSize);

int main(int argc, char **argv)
 {
	 for(;;){
	int FileSize = 0; 
	
	if (argc < 4) {
		printf("./myclient <IP ADDRESS> <PORT>[<filename>]\n");
		exit(1);
	}
	
	FileSize = Filesize(argv);
	printf("the size of the file is: %d\n",FileSize);
	printf("going into downloadfunction\n");
	downloadfunction(argv,FileSize);
	
	
	
	

	exit(0);
	 }
	}
	
	
int Filesize(char **argv){
	struct sockaddr_in servaddr;
	int sockfd =0;
	int filesize; 
	char flag;
	socklen_t servlen;

	char RECFSBUFF[20];
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
		printf("error: socket creation failed\n");
		exit(1);
	}
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	servlen = sizeof(servaddr);

/* 	if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))<0){
		printf("error: connecting so socket\n");
		exit(1);
	} */
	printf(" the file being opened is %s\n",argv[3]);
	if(sendto(sockfd,argv[3],strlen(argv[3]),0,(SA *)&servaddr,servlen)<0){
		printf("error: nothing was written to the server\n");
		exit(1);
	}
	
	if(recvfrom(sockfd,&flag,1,0, (SA *) &servaddr,&servlen)<0){
		printf("error:No file\n");
		exit(1);
	}else{
		printf("File Open Succesfully\n");
		//printf("%s\n", &flag);
	}
	
	if((sendto(sockfd,"0",1, 0, (SA *) &servaddr,servlen))<0){
		printf("error: write to server failed\n");
	}
	if(recvfrom(sockfd,&RECFSBUFF,sizeof(RECFSBUFF),0, (SA *) &servaddr, &servlen)<0){
		printf("error:reading filesize failed");
	}
	printf("RECFSBUFF is %s\n",&RECFSBUFF);
	filesize = atoi(RECFSBUFF);
	//sprintf(filesize,);
	//filesize = ntohl(recieved_val);
	printf("filesize %d \n", filesize);
	return filesize;
}

void downloadfunction(char **argv, int FileSize){
	FILE *fp;
	fd_set rfds;
	struct timeval tv;
	struct sockaddr_in servaddr;
	int size =0;
	int sockfd =0;
	int sizeofSB = 0;
	int n = 0;
	char flag;
/* 	char *data = calloc(info->size,sizeof(info->size)); */
	//threadinfo *info;
	char *data[MAXLINE]; 
	socklen_t servlen;

	
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
		printf("error: socket creation failed\n");
		exit(1);
	}
	//gonna have to change this up
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	printf("about to grab the server address length\n");
	servlen = sizeof(servaddr);
	printf("size of address is %d\n", servlen);

	
	/* if(connect(sockfd,(SA *)&servaddr,sizeof(servaddr))<0){
		printf("error: connecting so socket\n");
		exit(1);
	} */
	
	printf(" the file being opened is %s\n",argv[3]);
	if(sendto(sockfd,argv[3],strlen(argv[3]),0,(SA *)&servaddr, servlen)<0){
		printf("error: nothing was written to the server\n");
		exit(1);
	}
	
	if((recvfrom(sockfd,&flag,1,0, (SA *)&servaddr, &servlen))<0){
		printf("error:No file\n");
		exit(1);
	}else{
		printf("File Open Succesfully\n");
	}
	//above where all the connections and looking for files were taking place. below this comment is where the downloading of the files occur.
	
	if(sendto(sockfd,"1",1,0, (SA *)&servaddr, servlen)<0){
		printf("error: writing to 1 to server to start download failed.\n");
		exit(1);
	}
	sizeofSB = htonl(0);
	
	if((sendto(sockfd, &sizeofSB,sizeof(sizeofSB),0, (SA *)&servaddr, servlen))<0){
		printf("error: writing to socket failed\n");
		exit(1);
	}
	
	/* size = htonl(info->size); */
	size = FileSize;
	printf("filesize is: %d\n", size);
	
	if (sendto(sockfd,&size,sizeof(size),0, (SA *)&servaddr, servlen)<0){
		printf("error: writing to socket failed\n");
	}
	FD_ZERO(&rfds);
	FD_SET(sockfd,&rfds);
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	select(sockfd+1,&rfds,NULL,NULL,&tv);
	if(FD_ISSET(sockfd, &rfds)){
		if(recvfrom(sockfd,data,MAXLINE,0, (SA *)&servaddr, &servlen)< 0){
			printf("error: recieving data through network\n");
		}
	}
	printf("The Buffer Contains: %s\n", data);
	fp = fopen(argv[3],"w");
	fwrite(data,1,sizeof(data),fp);
	fclose(fp);
}


	
	
	
	//code that could possibly work in future
	/* 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("error could not connect socket");
		exit(1);
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	
	if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) <0 ){
		printf("error could not connect to server");
	}
	for ( ; ; ){
		printf(" client $ ");
		fgets(buffer,MAXLINE,stdin);
		
		//check for exit command if they match exit and close sockets
		if (strncmp(buffer, "exit",4) == 0){
			close(sockfd);
			printf ("Exit command received\n");
			return(0);
		}
		//write it to the server
		if(write(sockfd, buffer, strlen(buffer))<0){
		printf("error could not write to server");
		exit(1);
		}
		//read from the server
		if(read(sockfd,rcvline,MAXLINE)<0){
		printf("error could not read from server");
		exit(1);
		}
		printf("%s",rcvline);
		
	} */
