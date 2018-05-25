#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "structure.h"

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

int seeMemoryState(){

	key_t key = 6001;
	//Obtaining Access to shared memory
	int shmid =  shmget(key, 1, 0666);
	if(shmid<0)
	{
		perror("Reader Error: Access Problem");
		return 0;
	}
 
	//Attaching the shared memory
	 //char *shm = (shm = shmat(shmid, NULL, 0)); 
	 programa *memoria = (memoria = shmat(shmid, NULL, 0)); 
        
	if(memoria=="-1")
	{
		perror("Reader Error: Problem in attaching");
	        return 0;
	}

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
	printf("%i ",memoria->writer.sleep_time); 	
	printf("\n\n\n");
	return 0;
	
}

int main(int argc, char const *argv[])
{
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
				//see(MSG_MEM,FILE_MEM);
				break;
			case 3:
				//see(MSG_SEARCH,FILE_SEARCH);
				break;
			case 4:
				//see(MSG_WAIT, FILE_WAIT);
				break;
			case 5:
				//see(MSG_DEATH, FILE_DEATH);
				break;
			case 6:
				//see(MSG_FINISH, FILE_FINISH);
				break;

		}
	} while(choice != 9);
}

