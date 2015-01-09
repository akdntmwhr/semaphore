#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "ex11-1.h"

class_t class= {
	"1001", "120186", "C Language for Programmers", 15 };

#define NCHILD 3
int child[NCHILD];

char *shm_ptr;
int semid, shmid;
char ascsemid[10], ascshmid[10];
char pname[14];
void rpterror();

int main(int argc, char *argv[])
{
	int i;
	strcpy(pname, argv[0]);
	shm_init(); sem_init();
	for(i=0; i< NCHILD; i++) {
		child[i] = fork();
		switch(child[i]){
		case -1: rpterror("fork-failure");
			exit(1);
		case 0: sprintf(pname, "ex11-1b%d", i+1);
			execl("ex11-1b", pname, ascshmid, ascsemid, (char *) 0);
		}
	}
	wait_and_wrap_up();
}

shm_init()
{
	shmid=shmget(IPC_PRIVATE, sizeof(class), 0600|IPC_CREAT);
	if(shmid == -1) {
		perror("shmget failed"); exit(3);
	}
	shm_ptr = shmat(shmid, (char *) 0,0);
	if(shm_ptr == (char *) -1){
		perror("shmat failed"); exit(4);
	}
	memcpy(shm_ptr, (char *) &class, sizeof(class));
	sprintf(ascshmid, "%d", shmid);
}
sem_init(){
	if((semid=semget(IPC_PRIVATE, 1, 0600|IPC_CREAT)) == -1) {
		perror("semget failed"); exit(5);
	}
	if((semctl(semid, 0, SETVAL, 1)) == -1){
		printf("parent : semctl, SETVAL failed\n"); exit(6);
	}
	sprintf(ascsemid, "%d",semid);
}

wait_and_wrap_up(){
	int wait_rtn, w, ch_active = NCHILD;
	while(ch_active > 0){
		wait_rtn = wait((int *) 0);
		for(w=0; w<NCHILD; w++)
			if(child[w] == wait_rtn){
				ch_active--;
				break;	
			}
	}
	printf("Parent removing shm and sem\n");
	shmdt(shm_ptr);
	shmctl(shmid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID, 0);
	exit(0);
}

void rpterror(string)
char *string;
{
	char errline[50];
	sprintf(errline, "%s %s", string, pname);
	perror(errline);
}
