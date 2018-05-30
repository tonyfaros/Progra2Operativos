#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "structure.h"
programa *memoria;
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int loadMemory(){

	key_t key = 6006;
	//Obtaining Access to shared memory
	int shmid =  shmget(key, 1, 0666);
	if(shmid<0)
	{
		perror("Reader Error: Access Problem");
		return 0;
	}
 
	//Attaching the shared memory
	 memoria = (memoria = shmat(shmid, NULL, 0)); 
        
	if(memoria=="-1")
	{
		perror("Reader Error: Problem in attaching");
	        return 0;
	}

	
	return 0;
	
}

int seeMemoryState(){

	//Reading what writer inserted into the shared memory...!!
	printf("\nEstado de la memoria:\n");	
	printf("\nReader Read: ");	
	char buff[100];
	int count=0;
	char c;
	for (int i=0;i<memoria->memory_size;i++)
        {	
			putchar('\n');	
			for(int j = 0; j<strlen(memoria->lines[i].data);j++){
				putchar(memoria->lines[i].data[j]);
			}
	}   
	printf("\n\n\n");
	return 0;
}

void seeWritersState(){
	printf("Estado de Writers:\n\n");
	padre writer = memoria->writer;

	// 0->dormido  1->wait  2->ejecucion

	//ejecutando
	printf("Procesos accediendo al archivo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (writer.procesos[contador].status == 2)
			printf("Proceso %d\n",contador);
	}
	

	//sleep
	printf("\nProcesos durmiendo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (writer.procesos[contador].status == 0)
			printf("Proceso %d\n",contador);
	}

	//block
	printf("\nProcesos bloqueados:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (writer.procesos[contador].status == 1)
			printf("Proceso %d\n",contador);
	}
	printf("\n\n\n");

}
void seeReadersState(){
	printf("Estado de Readers:\n\n");
	padre reader = memoria->reader;
	int contador = 0;

	// 0->dormido  1->wait  2->ejecucion

	//ejecutando
	printf("Procesos accediendo al archivo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (reader.procesos[contador].status == 2)
			printf("Proceso %d\n",contador);
	}
	

	//sleep
	printf("\nProcesos durmiendo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (reader.procesos[contador].status == 0)
			printf("Proceso %d\n",contador);
	}

	//block
	printf("\nProcesos bloqueados:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (reader.procesos[contador].status == 1)
			printf("Proceso %d\n",contador);
	}
	printf("\n\n\n");
}
void seeEgoistasState(){
	printf("Estado de Readers Egoistas:\n\n");
	padre egoista = memoria->egoista;
	int contador = 0;

	// 0->dormido  1->wait  2->ejecucion

	//ejecutando
	printf("Procesos accediendo al archivo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (egoista.procesos[contador].status == 2)
			printf("Proceso %d\n",contador);
	}
	

	//sleep
	printf("\nProcesos durmiendo:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (egoista.procesos[contador].status == 0)
			printf("Proceso %d\n",contador);
	}

	//block
	printf("\nProcesos bloqueados:\n");
	for(int contador = 0;contador<memoria->writer.cant_hijos; contador++){
		if (egoista.procesos[contador].status == 1)
			printf("Proceso %d\n",contador);
	}
	printf("\n\n\n");
}


int main(int argc, char const *argv[])
{
	loadMemory();
	int choice;
	do{
		printf("PROGRAMA ESPIA\n");
        printf("#####################\n");
		printf("1 - Estado de la memoria\n");
		printf("2 - Estado Writers\n");
		printf("3 - Estado Readers\n");
		printf("4 - Estado Readers Egoistas\n");
		printf("5 - Salir\n");
        printf("Digite una opcion\n");
		scanf("%d",&choice);
		printf("\n");

		switch(choice){
			case 1:
				seeMemoryState();
				break;
			case 2:
				seeWritersState();
				break;
			case 3:
				seeReadersState();
				break;
			case 4:
				seeEgoistasState();
				break;

		}
	} while(choice != 5);
}

