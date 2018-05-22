#include "controlador.h"

int main(int argc, char const *argv[])
{
	int memory = getMemorySize(FILE_SIZE);
	int choice;
	do{
		printf("PROGRAMA ESPIA\n");
        printf("#####################\n");
		printf("1 - Estado de la memoria\n");
		printf("2 - Estado Writers\n");
		printf("3 - Estado Readers\n");
		printf("4 - Estado Readers Egoistas\\n");
		printf("9 - Salir\n");
        printf("Digite una opcion\n");
		scanf("%d",&choice);
		printf("\n");

		switch(choice){
			case 1:
				seeMemoryState(FILEKEY, KEY, memory);
				break;
			case 2:
				see(MSG_MEM,FILE_MEM);
				break;
			case 3:
				see(MSG_SEARCH,FILE_SEARCH);
				break;
			case 4:
				see(MSG_WAIT, FILE_WAIT);
				break;
			case 5:
				see(MSG_DEATH, FILE_DEATH);
				break;
			case 6:
				see(MSG_FINISH, FILE_FINISH);

		}
	} while(choice != 9);
}

