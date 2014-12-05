#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_BUFFER 1024

typedef struct _message{
	long message_type;
	struct sockaddr_in addr;
	char message_text[MAX_BUFFER];
}message_t;


struct sockaddr_in servaddr;
int sock;
int msqid;



void thr_errquit(char *message, int errcode)
{
        printf("%s : %s\n", message, strerror(errcode));
        pthread_exit(NULL);
}



void *echo_recv(void *arg)
{
	int nbytes, status, len = sizeof(struct sockaddr);
	message_t pmessage;
	int size;
	size = sizeof(pmessage) - sizeof(long);
	pmessage.message_type = pthread_self();
	while(1) {
		nbytes = recvfrom(sock, pmessage.message_text, MAX_BUFFER, 0,
		(struct sockaddr *)&pmessage.addr, &len);
		if(nbytes <0)
			thr_errquit("recvfrom fail", errno);
		pmessage.message_text[nbytes] = 0;
		printf("recv thread = %ld\n", pthread_self());
		if(msgsnd(msqid, &pmessage, size, 0) == -1)
			thr_errquit("메시지보냄 실패", errno);
	}
}

void *echo_resp(void *arg)
{
	message_t pmessage;
	int nbytes, len = sizeof(struct sockaddr);
	int size;
	size = sizeof(pmessage) - sizeof(long);
	pmessage.message_type = 0;
	while(1)
	{
		if(msgrcv(msqid,(void *)&pmessage, size, 0, 0) < 0)
		{
			perror("msgrcv fail"); exit(0);
		}
		nbytes = sendto(sock, pmessage.message_text, strlen(pmessage.message_text),0,
		(struct sockaddr*)&pmessage.addr, len);
		if(nbytes < 0)
			thr_errquit("전송 실패", errno);

		printf("응답 스레드 = %ld\n\n", pthread_self());
		pmessage.message_text[0] = 0;
	}
}

void errquit(char *message)
{
	perror(message);
	exit(-1);
}



int main(int argc, char *argv[])
{
	pthread_t tid[6];
	struct sockaddr_in cliaddr;
	int port, status, i, len = sizeof(struct sockaddr);
	key_t msqkey;
	if(argc!=3)
	{
		printf("Usage: %s msgkey prot\n", argv[0]);
		exit(1);
	}
	msqkey = atoi(argv[1]);
	port = atoi(argv[2]);
	


	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		errquit("소켓 실패");

	bzero(&servaddr, len);
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_family=AF_INET;
	bind(sock, (struct sockaddr*)&servaddr, len);

	for(i=0; i<5; i++)
		if((status=pthread_create(&tid[i], NULL, echo_recv, NULL)!=0))
			thr_errquit("pthread_create", status);
	
	if((status=pthread_create(&tid[5], NULL, echo_resp, NULL))!=0)
		thr_errquit("pthread_create", status);
	for(i=0; i<6; i++)
		pthread_join(tid[i], NULL);

	msgctl(msqid, IPC_RMID, 0);
	return 0;
}
