#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 127

int main(int argc, char *argv[])
{
	struct sockaddr_in servaddr, cliaddr;
	int listen_sock, accp_sock, addrlen=sizeof(cliaddr), nbyte;
	char buf[MAXLINE+1];	

	if(argc != 2)
	{
		printf("usage: %s port\n", argv[0]);
		exit(0);
	}

	if((listen_sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{	
		perror("socket fail");
		exit(0);
	}

	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if(bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
	{
		perror("bind fail");
		exit(0);
	}

	if(bind(4, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0 )
		perror("test1 bind fail:");
	if(bind(1, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("test2 bind fail.");

	servaddr.sin_port = htons(80);
	if(bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("test3 bind fail: ");
	servaddr.sin_port = htons(200);
	if(bind(listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
		perror("test4 bind fail: ");

	return 0;
}	
