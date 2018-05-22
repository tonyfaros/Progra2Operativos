#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "structure.h"






int main(int argc, char *argv[]){

	Shared_mem *shared_mem = (Shared_mem*)malloc(sizeof(Shared_mem));

	int total_lines = size(shared_mem);

	key_t key = ftok("shmfile",21);

	int shmid = shmget(key, total_lines, 0777 | IPC_CREAT);

	char *str = (char*) shmat(shmid, (void*)0,0);

	int i = 0;

	for(i = 0; i< total_lines; i++){
		str[i] = '0';
		i = i+1;
	}
	return (EXIT_SUCCESS);
}
