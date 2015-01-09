#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#define MSG_SIZE 30

int main(int argc, char *argv[])
{
	key_t key;
	static struct sembuf wait_produced = {0,-1,0};
	static struct sembuf signal_consumed = {1, 1, 0};
	int semid, shmid;
	char *message;
	int rtn;

	if((key = ftok(getenv("HOME"), 'u')) == (key_t) -1) {
		fprintf(stderr, "ftok key formation error\n");
		exit(1);
	}

	semid = semget(key, 2, IPC_CREAT | 0660);
	if(semid == -1){
		perror("producer semget():");
		exit(2);
	}
	shmid = shmget(key, MSG_SIZE, IPC_CREAT | 0660);
	if(semid == -1) {
		perror("producer shmget():");
		exit(3);
	}
	message = shmat(shmid, (char *) 0, SHM_RDONLY);
	while(1){
		rtn = semop(semid, &wait_produced, 1);
		if(rtn == -1){
			perror("consumer - semop on wait_consumed");
			break;
		}
		printf("%s received: %s\n", argv[0], message);
		semop(semid, &signal_consumed, 1);
	}
	shmdt(message);
}
