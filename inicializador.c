#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "structure.h"




int main(int argc, char *argv[]){

	int parametro = atoi(argv[1]);
	char total_lines[12];
	sprintf(total_lines, "%d", parametro);


	key_t key = 6006;
	//Obtaining Access to shared memory
	int shmid = shmget(key, sizeof(programa), 0777 | IPC_CREAT);
	
	if(shmid<0)
	{
		perror("Writer Error: Access Problem");
		return 0;
	}
	
	//Attaching the shared memory
	programa *memoria = shmat(shmid, NULL, 0);
	
	memoria->memory_size = parametro;
	memoria->lineas_vacias = parametro;
	printf("linea %i\n\n", memoria->lineas_vacias);
	memoria->finalizar = 1;

	memoria->sem_reader=sem_open("/reader", O_CREAT | O_EXCL, 0644, 1);


	//se llenan las lineas vacias
	char vacio[]="vacio";
	for (int j = 0; j<parametro; j++){
		for (int i = 0; i<strlen(vacio); i++)
		{
			memoria->lines[j].data[i]=vacio[i];
		}
	}
	
   	printf("Memoria apartada con exito\n");
	return 0;
}
//ipcrm -M 6000
