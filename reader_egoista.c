#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <mach/semaphore.h>

#include "structure.h"

void escribir_bitacora(char *msj);
programa *memoria;

unsigned int *contador;

pthread_mutex_t *mutex_archivo;
pthread_mutexattr_t *attr_mutex_archivo;

semaphore_t *sem_egoista;
time_t mytime;
int num_linea;

/*
 * READER
 */

void *egoista_function(void *arg)
{   
    int PID = *((int *) arg);
    int contador = 1;
    while(memoria->finalizar){
        memoria->reader.procesos[PID].status = 1;
        semaphore_wait(sem_egoista); 
        
        while(memoria->contador_egoista == 3){
            if(!pthread_mutex_trylock(&mutex_archivo)){
                pthread_mutex_unlock(&mutex_archivo);
                break;
            }

        }
        while(memoria->lineas_vacias == memoria->memory_size);
        
        num_linea = rand() % (memoria->memory_size-1 + 1 - 0) + 0;
        
        while(strcmp (memoria->lines[num_linea].data, "vacio") == 0){
            //printf("\n%d\n",strcmp (memoria->lines[num_linea].data, "vacio"));
            num_linea = rand() % (memoria->memory_size-1 + 1 - 0) + 0;
        }
        pthread_mutex_lock(&mutex_archivo);
        memoria->contador_egoista = memoria->contador_egoista+1;
        
        
        mytime = time(NULL);
        struct tm *info;
        char tiempo[50];
        char mensaje[80]= "";
        char aux[12];


        //se obtiene la fecha
        time( &mytime );
        info = localtime( &mytime );
        strftime(tiempo,80,"%x - %I:%M%p", info);
        
        memoria->reader.procesos[PID].status = 2;

        printf("\nlectura en linea %i\n", num_linea);
        printf("proceso %i leyendo\n", PID);

        
        for (int i = 0; i<strlen(memoria->lines[num_linea].data); i++)
		{
			mensaje[i]=memoria->lines[num_linea].data[i];
            //putchar(memoria->lines[num_linea].data[i]);
		}
        
        printf("Mensaje: %s\n",mensaje);

        //escritura en bitacora
        char bitacora[500];
        sprintf(aux, "%d", PID);
        strcpy(bitacora,aux);
        strcat(bitacora, " ~ ");

        strcat(bitacora, tiempo);
        strcat(bitacora," ~ mensaje->");
        
        strcat(bitacora,mensaje);
        strcat(bitacora,"\n");
        escribir_bitacora(bitacora);


        for (int i = 0; i<strlen(mensaje); i++)
		{
			memoria->lines[num_linea].data[i]=(char) 0;
		}

        char vacio[]="vacio";
		for (int i = 0; i<strlen(vacio); i++)
		{
			memoria->lines[num_linea].data[i]=vacio[i];
		}
	
 
        fflush(stdout);
        sleep(memoria->reader.execution_time);
        

        pthread_mutex_unlock(&mutex_archivo);
        semaphore_signal(sem_egoista);
        memoria->reader.procesos[PID].status = 0;
        printf("proceso %i durmiendo\n", PID);
        sleep(memoria->reader.sleep_time);      

    }
            
    return NULL;
}


int main(int argc, char** argv) {

    int cantidad_egoistas = atoi(argv[1]);
    int tiempo_sleep= atoi(argv[2]);
    int tiempo_read = atoi(argv[3]);

    pthread_t tid[cantidad_egoistas];
	
    key_t key = 6006;
	//Obtaining Access to shared memory
	int shmid =  shmget(key, 1, 0666);
	if(shmid<0)
	{
		perror("Reader Error: Access Problem");
		return 0;
	}
 
	//Attaching the shared memory
	//programa *memoria = (memoria = shmat(shmid, NULL, 0)); 
    memoria = (memoria = shmat(shmid, NULL, 0));
        
	if(memoria=="-1")
	{
		perror("Reader Error: Problem in attaching");
	        return 0;
	}
    memoria->egoista.cant_hijos = cantidad_egoistas;
    memoria->egoista.sleep_time = tiempo_sleep;
    memoria->egoista.execution_time = tiempo_read;


    attr_mutex_archivo = memoria->attr_mutex_archivo;
    mutex_archivo = memoria->mutex_archivo;

    pthread_mutexattr_init(&attr_mutex_archivo);
    pthread_mutex_init(&mutex_archivo, &attr_mutex_archivo);

    mach_port_t self = mach_task_self();
    semaphore_create(self, &sem_egoista, SYNC_POLICY_FIFO, 1);
    //sem_reader = memoria->sem_reader;
    
    
    
    
    int i;

    for(i = 0; i<cantidad_egoistas; i++){
        memoria->reader.procesos[i].PID = i;
        pthread_create(&tid[i], NULL, (void*)egoista_function, &i);
        sleep(1);
    }

    for(i=0; i<cantidad_egoistas; i++)
    {
        pthread_join(tid[i], NULL);
    }
    semaphore_destroy(self, sem_egoista);

    return 0;
}


void escribir_bitacora(char *msj){
    FILE *bitacora;
    bitacora = fopen ("bitacora.txt", "a");  
    fprintf(bitacora,"Egoista-> %s\n",msj);
    fclose(bitacora);

}
//ipcrm -M 6002