#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	struct sockaddr_in6 svrsin6;
	struct hostennt *hp;
	char ipv6_addr[16];

	char addr6_str[40];	
	char buf[256];

	int sockfd;	
	int clilen;

	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("socket create error:");
		exit(0);
	}

	svrsin6.sin6_family = AF_INET6;	
	svrsin6.sin6_flowinfo = 0;
	svrsin6.sin6_port = htons(atoi(argv[1]));
	svrsin6.sin6_addr = in6addr_loopback;

	memcpy((void *)&svrsin6.sin6_addr, (void *)&ipv6_addr, 16);
	inet_ntop(AF_INET, (void *)&svrsin6.sin6_addr, addr6_str, 40);

	printf("%s\b", addr6_str);
	printf("connect...\n");

	clilen = sizeof(svrsin6);
	if(connect(sockfd, (struct sockaddr *)&svrsin6, clilen) < 0)
	{
		perror("connect error:");
		exit(0);
	}
	memset(buf, 0x00, 256);
	read(0, buf, 256);
	write(sockfd, buf, 256);
	read(sockfd, buf, 256);
	printf("-->%s", buf);

	printf("Connect Success\n");
	close(sockfd);
	exit(0);
}
