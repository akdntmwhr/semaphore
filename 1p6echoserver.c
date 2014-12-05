#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	struct sockaddr_in6 sin6, clisin6;
	int sockfd, clisockfd;
	int clilen = sizeof(clisin6);
	char buf[256];
	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd <0)
	{
		perror("socket create error:");	
		exit(0);
	}
	
	sin6.sin6_family = AF_INET6;
	sin6.sin6_flowinfo = 0;
	sin6.sin6_port = htons(atoi(argv[1]));
	
	sin6.sin6_addr = in6addr_any;

	if(bind(sockfd, (struct sockaddr *)&sin6, sizeof(sin6))== -1)
	{
		perror("Bind error");
		exit(0);
	}

	if(listen(sockfd, 5) == -1)
	{
		perror("Listen error");
		exit(0);
	}

	while(1)
	{
		clisockfd = accept(sockfd, (struct sockaddr *)&clisin6, (socklen_t *)&clilen);
		memset(buf, 0x00, 256);
		read(clisockfd, buf, 256);
		write(clisockfd, buf, 256);	
		
		close(clisockfd);
	}
}
