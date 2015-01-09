#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "ex11-1.h"

class_t *class_ptr;
char *memptr,  *pname;
int semid, shmid, ret;
struct sembuf lock = {0,-1,0};
struct sembuf unlock = {0,1,0};
void rpterror();

int main(int argc, char *argv[])
{
	if(argc < 3){
		fprintf(stderr, "Usage: %s shmid semid\n", argv[0]);
		exit(1);
	}
	pname = argv[0];
	sscanf(argv[1], "%d", &shmid);
	memptr = shmat(shmid, (char *) 0,0);
	if(memptr ==(char *) -1){
		rpterror("shmat failed");
		exit(2);
	}
	class_ptr = (class_t *) memptr;
	sscanf(argv[2], "%d", &semid);
	sell_seats();
	ret = shmdt(memptr);
	exit(0);
}

sell_seats(){
	int all_out = 0;
	srand((unsigned) getpid());
	while(! all_out){
		if(semop(semid, &lock, 1) == -1){
			rpterror("Semop lock failed");
			exit(4);
		}
		if(class_ptr ->seats_left > 0){
			class_ptr->seats_left--;
			printf("%s SOLD SEAT -- %2d left\n", pname, class_ptr->seats_left);
		} else {
			all_out++;
			printf("%s sees no seats left\n", pname);
		}
		ret = semop(semid, &unlock, 1);
		if(ret == -1) {
			rpterror("semop unlock failed");
			exit(4);
		}
		sleep((unsigned) rand()%10+1);
	}
}

void rpterror(string)
char *string;
{
	char errline[50];
	sprintf(errline, "%s %s",string, pname);
	perror(errline);
}
