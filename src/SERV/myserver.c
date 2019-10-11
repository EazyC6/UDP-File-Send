#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/time.h>
#define	SA	struct sockaddr
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define	MAXLINE		4096	/* max text line length */
//helper functions 

//code from the original files given by the textbooks (for use if need be)
//if ( (childpid = fork()) == 0) { /*child process*/
			//close(listenfd); /*close listening socket*/
			//str_echo(connfd); /* process the request */
			//exit (0);
		//}
		
		
		
		
//old code from the first lab this tackles all the command proccessing (may not be needed)
			/* while (fgets(path,MAXLINE,fp) != NULL){
				strncat(buffer,path,strlen(path));
			}
			//bzero(buffer,strlen(buffer));
			//print buffer to string and to output 
			printf("%s",buffer);
			//write this buffer to the connected client 
			write(connfd,buffer,strlen(buffer));
			//zero out the buffer to accept new feedback
     		bzero(buffer,strlen(buffer));
			//close the file. */
int filesize(FILE *fp);

int main(int argc, char **argv)
{
	int listenfd, size_of_file,recieved,offset, count,Fread;
	fd_set rfds;
	struct timeval tv;
	//int result;
	//int retval;
	int n;
	//int connfd;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	char FSBUF[20];
	//char buffer[MAXLINE];
	char file[MAXLINE+1];
	FILE *fp;
	//char path[MAXLINE];
	char flag;
	
	
	
	if(argc < 2){
		printf("usage: filename, PORT_NUM \n");
		exit(0);
	}
	
 	listenfd = socket (AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
	servaddr.sin_port = htons (atoi(argv[1]));
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr))< 0){
		printf("error:could not bind server\n");
		exit(1);
	}
	
	//listen(listenfd, LISTENQ);
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		/* connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
		   if (connfd <0){
		   printf("error:could not accept connection.\n");
		   } */

		if ((n = recvfrom(listenfd, file, MAXLINE,0,(SA *) &cliaddr,&clilen))>0){
			file[n] = 0; 
		}
		printf("filename %s\n", file);
		//this is to run the  commands
		fp = fopen(file,"r");
		if(fp == NULL) {
			sendto(listenfd,"1",1,0,(SA *) &cliaddr, clilen);
			printf("Error: Failed to Open File, please provide a new file\n");
			continue; 
		}
		else{
			sendto(listenfd, "0",1,0,(SA *) &cliaddr,clilen);

			if(recvfrom(listenfd,&flag,1,0, (SA *)&cliaddr,&clilen)>0){
				printf("%c\n",flag);
			}
			if (strncmp(&flag,"0",1) == 0){
				printf("flag %c: file size return\n", flag);
				//getting the file size
				if(fseek(fp,0,SEEK_END) != 0){
					printf("error seeking through file\n");
				}
				if((size_of_file = ftell(fp)) == -1L){
					printf("error returning current value of the position indicator\n");
				}
				//size_of_file = filesize(fp);
				printf("filesize is: %d\n", size_of_file);
				//size_of_file_temp =  htonl(size_of_file);
				//printf("size_of_file_temp %d\n",size_of_file_temp);
				sprintf(FSBUF,"%d", size_of_file);
				printf("%s\n",FSBUF);
				//sending the file to the client
				if(sendto(listenfd,&FSBUF,sizeof(FSBUF),0,(SA *) &cliaddr,clilen)<0){
					printf("error:could not write to socket\n");
					exit(1);
				}
				fclose(fp);
			}else if(strncmp(&flag,"1",1) == 0){
				printf("listenfd is: %d\n",listenfd);
				offset = 0; 
				count = 0;
				//do the recieving of the downloads here.
				printf("flag %c recieved: Download Starting Soon\n", flag);
				recieved  = 0;
				FD_ZERO(&rfds);
				FD_SET(listenfd,&rfds);
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				select (listenfd+1,&rfds,NULL,NULL,&tv);
				printf("listenfd is now: %d\n",listenfd);
				if(FD_ISSET(listenfd, &rfds)){
					//printf("i made it!\n");
					if(recvfrom(listenfd,&recieved,sizeof(recieved),0, (SA *) &cliaddr, &clilen)>0){
						offset = ntohl(recieved);
						printf("Offeset %d\n",offset);
					}			
					if (recvfrom(listenfd,&recieved,sizeof(recieved),0, (SA *) &cliaddr, &clilen)>0){
							count = ntohl(recieved); 
							printf("count is: %d\n", count);
					}
				}
				fseek(fp,offset,SEEK_SET);
				printf("I made it here\n");
				char buffer[MAXLINE];
				printf("I made it here 2\n");
				Fread = fread(buffer,1,count,fp);
					printf("error reading from file\n");
				
				printf("%d\n",Fread);
				printf("The buffer contains:%s\n",buffer);
				if(sendto(listenfd,buffer,Fread,0, (SA *) &cliaddr, clilen)<0){
					printf("error sending\n");
				}
				printf("Data in buffer Sent\n");
				fclose(fp);
				


				//fclose(fp);

			}
		}
		//fclose(fp);

		// close(connfd); /* parent closes connected socket */
	}
}

//helper functions

/* int filesize(FILE *fp){
   int size = 0; 
   int prev = 0;
   if ((prev = ftell(fp))  == -1L){
   printf("error returning current file position\n");
   }
   if(fseek(fp,0L,SEEK_END) == 0){
   printf("made it through\n");
   }
   if((size = ftell(fp)) == -1L){
   printf("error returning current file position\n");
   }
   if(fseek(fp,prev,SEEK_SET) < 0 ){
   printf("made it through\n");
   }
   printf("size: %d\n", size);
   return size;
   } */

