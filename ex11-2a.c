#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#define MSG_SIZE 30
#define MAX 5

main(){
	char *getenv();
	key_t ftok();
	key_t key;
	void perror(), exit();

	static struct sembuf wait_consumed = {1, -1, 0};
	static struct sembuf signal_produced = {0, 1, 0};
	int semid, shmid;
	char *message;
	int i;
	int *semvalue;
	if((key = ftok(getenv("HOME"), 'u')) ==(key_t) -1) {
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
		perror("producer shmget():");	exit(3);
	}
	sem_getvalue(semid, semvalue);
	printf("sem : %d\n", *semvalue);
	message = shmat(shmid, (char *) 0, 0);
		
	for(i=1; i<MAX; i++) {
		if(i>1){ semop(semid, &wait_consumed, 1);
		sem_gevalue(semid, semvalue);
		printf("wait_consumed %d", *semvalue);}
		sprintf(message, "message %d", i);
		semop(semid, &signal_produced, 1);
	}
	shmdt(message);

	//sleep(5);
	shmctl(shmid, IPC_RMID, (struct shmid_ds *) 0);
	shmctl(shmid, 0, IPC_RMID);
}
