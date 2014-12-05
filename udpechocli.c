#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECHOMAX 255

int main(int argc, char *argv[])
{
	int sock;	
	struct sockaddr_in echoServAddr;
	struct sockaddr_in fromAddr;
	unsigned short echoServPort;
	unsigned int fromSize;
	char *servIP;
	char *echoString;
	char echoBuffer[ECHOMAX+1];
	int echoStringLen;
	int respStringLen;

	if((echoStringLen=strlen(echoString))>ECHOMAX)
	{
		printf("Echo word too long");
		exit(0);
	}

	if(argc=4)
		echoServPort=atoi(argv[3]);
	else
		echoServPort=7;

	if((sock=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP))<0)
	{
		printf("socket() failed");
		exit(0);
	}

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family=AF_INET;
	echoServAddr.sin_addr.s_addr=inet_addr(servIP);
	echoServAddr.sin_port=htons(echoServPort);

	if(sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr))!=echoStringLen){
		printf("sendto() sent a different number of bytes than expected");
		exit(0);
	}

	fromSize=sizeof(fromAddr);
	
	if((respStringLen=recvfrom(sock, echoBuffer, ECHOMAX, 0, (struct sockaddr *)&fromAddr, &fromSize))!=echoStringLen){
		printf("recvfrom() failed");
		exit(0);
	}

	if(echoServAddr.sin_addr.s_addr !=fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		exit(1);
	}

	echoBuffer[respStringLen]='\0';
	printf("Received: %s\n", echoBuffer);
	close(sock);
	exit(0);
}
