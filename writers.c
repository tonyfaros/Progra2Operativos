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

#include "structure.h"

void escribir_bitacora(char *msj);
unsigned int *contador;
programa *memoria;

pthread_mutex_t *mutex_archivo;
pthread_mutexattr_t *attr_mutex_archivo;
sem_t *sem_writer;
time_t mytime;
int num_linea = 0;



void *writer_function(void *arg)
{   
    int PID = *((int *) arg);
    int contador = 1;
    while(1){
        memoria->writer.procesos[PID].status = 3;
        
        sem_wait(memoria->sem_writer);
        while(memoria->lineas_vacias <= 0);
        
        while(strcmp (memoria->lines[num_linea].data, "vacio") != 0){
            num_linea++;
        
        }
        pthread_mutex_lock(&mutex_archivo);
        
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
        //printf("Formatted date & time : |%s|\n", tiempo );

        strcat(mensaje, tiempo);
        strcat(mensaje, " -- ");
        sprintf(aux, "%d", memoria->memory_size-memoria->lineas_vacias);
        strcat(mensaje, aux);
        //printf("MENSAJE|%s|\n", mensaje);
        memoria->writer.procesos[PID].status = 2;

        printf("escritura en linea %i\n\n", memoria->memory_size-memoria->lineas_vacias);

        printf("proceso %i escribiendo\n", PID);
        
        
        for (int i = 0; i<strlen(mensaje); i++)
		{
			memoria->lines[memoria->memory_size-memoria->lineas_vacias].data[i]=mensaje[i];
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
        memoria->writer.procesos[PID].status = 0;
        sem_post(memoria->sem_writer);
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
    pthread_t block;
	
    key_t key = 6002;
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
    memoria->sem_writer = sem_open("/writer", O_CREAT | O_EXCL, 0644, 1);
    
    sem_writer = memoria->sem_writer;
    
    int i;
    //pthread_create(&block, NULL, block_, NULL);
    for(i = 0; i<cantidad_writers; i++){
        memoria->writer.procesos[i].PID = i;
        //sem_wait(memoria->sem_writer);
        pthread_create(&tid[i], NULL, (void*)writer_function, &i);
        sleep(1);
        //printf("proceso %i\n", i);
    }

   
    for(i=0; i<cantidad_writers; i++)
    {
        pthread_join(tid[i], NULL);
        printf("hola");
    }

    /*

    int n_procesos = 2;
    int t_sleep = 4;
    int t_write = 2;    
    key_t key = ftok("shmfile",21);        
    int shmid = shmget(key,1,0666|IPC_CREAT);  
    pthread_t writer_array[4];    
    int i = 0;        
    while(i<n_procesos){    
        Writer *w = malloc(sizeof(Writer));        
        w->id = i;
        w->shmid = shmid;
        w->tiempo_sleep = t_sleep;
        w->tiempo_write = t_write;        
        pthread_create(&writer_array[i], NULL, writer_function, (void*) w);                                
        i=i+1;
    }
    pthread_join(writer_array[0], NULL); 
                   
    
    */
    //while(memoria->finalizar);     
    return 0;
}

void escribir_bitacora(char *msj){
    FILE *bitacora;
    bitacora = fopen ("bitacora.txt", "a");  
    fprintf(bitacora,"Writer-> %s\n",msj);
    fclose(bitacora);
}
