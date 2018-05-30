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
unsigned int *contador;
programa *memoria;

pthread_mutex_t *mutex_archivo;
pthread_mutexattr_t *attr_mutex_archivo;

semaphore_t *sem_writer;
time_t mytime;
int num_linea = 0;



void *writer_function(void *arg)
{   
    int PID = *((int *) arg);
    int contador = 1;
    while(memoria->finalizar){
        memoria->writer.procesos[PID].status = 1;
        
        semaphore_wait(sem_writer);
        while(memoria->lineas_vacias <= 0);
        //while(PID==1);
        if (num_linea == memoria->memory_size)
            num_linea = 0;
        while(strcmp (memoria->lines[num_linea].data, "vacio") != 0){
            num_linea=num_linea+1;
        
        }
        pthread_mutex_lock(&mutex_archivo);
        memoria->contador_egoista = 0;
		mytime = time(NULL);
		

        struct tm *info;
        char tiempo[50];
        char mensaje[80];
        char aux[12];
	    sprintf(aux, "%d", PID);

        strcpy(mensaje, aux);
        strcat(mensaje, " -- ");
        
        //se obtiene la fecha
        time( &mytime );
        info = localtime( &mytime );
        strftime(tiempo,80,"%x - %I:%M%p", info);
        

        strcat(mensaje, tiempo);
        strcat(mensaje, " -- ");
        sprintf(aux, "%d", memoria->memory_size-memoria->lineas_vacias);
        strcat(mensaje, aux);
        
        memoria->writer.procesos[PID].status = 2;

        printf("escritura en linea %i\n\n", memoria->memory_size-memoria->lineas_vacias);

        printf("proceso %i escribiendo\n", PID);
        
        
        for (int i = 0; i<strlen(mensaje); i++)
		{
			memoria->lines[num_linea].data[i]=mensaje[i];
		}
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
        memoria->lineas_vacias = memoria->lineas_vacias-1;
 
        fflush(stdout);
        sleep(memoria->writer.execution_time);
        
        
        pthread_mutex_unlock(&mutex_archivo);
        semaphore_signal(sem_writer);
        memoria->writer.procesos[PID].status = 0;
        printf("proceso %i durmiendo\n", PID);
        sleep(memoria->writer.sleep_time);      

    }
            
    return NULL;
}

int main(int argc, char** argv) {
    
    int cantidad_writers = atoi(argv[1]);
    int tiempo_sleep= atoi(argv[2]);
    int tiempo_write = atoi(argv[3]);
    pthread_t tid[cantidad_writers];
	
    key_t key = 6006;
	//Obtaining Access to shared memory
	int shmid =  shmget(key, 1, 0666);
	if(shmid < 0)
	{
		perror("Reader Error: Access Problem");
		return 0;
	}
 
	//Attaching the shared memory
	memoria = (memoria = shmat(shmid, NULL, 0)); 
        
	if(memoria == "-1")
	{
		perror("Reader Error: Problem in attaching");
	        return 0;
	}
    memoria->writer.cant_hijos = cantidad_writers;
    memoria->writer.sleep_time = tiempo_sleep;
    memoria->writer.execution_time = tiempo_write;

    attr_mutex_archivo = memoria->attr_mutex_archivo;
    mutex_archivo = memoria->mutex_archivo;
    
    
    pthread_mutexattr_init(&attr_mutex_archivo);
    pthread_mutex_init(&mutex_archivo, &attr_mutex_archivo);

    //sem_writer = sem_open("/writer", O_CREAT | O_EXCL, 0644, 1);
    mach_port_t self = mach_task_self();
    semaphore_create(self, &sem_writer, SYNC_POLICY_FIFO, 1);
    
    //sem_writer = memoria->sem_writer;
    
    int i;
    for(i = 0; i<cantidad_writers; i++){
        memoria->writer.procesos[i].PID = i;
        pthread_create(&tid[i], NULL, (void*)writer_function, &i);
        sleep(1);
    }

   
    for(i=0; i<cantidad_writers; i++)
    {
        pthread_join(tid[i], NULL);
    }
    semaphore_destroy(self, sem_writer);

        
    return 0;
}

void escribir_bitacora(char *msj){
    FILE *bitacora;
    bitacora = fopen ("bitacora.txt", "a");  
    fprintf(bitacora,"Writer-> %s\n",msj);
    fclose(bitacora);
}
