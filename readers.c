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

semaphore_t *sem_reader;
time_t mytime;
int num_linea = 0;
int entrar = 0; //0 no y 1 es si

/*
 * READER
 */

void *reader_function(void *arg)
{   
    int PID = *((int *) arg);
    int contador = 1;
    while(memoria->finalizar){
        memoria->reader.procesos[PID].status = 1;
        semaphore_wait(sem_reader);
        do{
            while(1){
                if(!entrar){
                    while(pthread_mutex_trylock(&mutex_archivo));
                    printf("memoria bloqueada");
                }
                
                if(memoria->lineas_vacias < memoria->memory_size){
                    
                    entrar = 1;
                    break;
                }
                else{
                    pthread_mutex_unlock(&mutex_archivo);
                    printf("memoria DESbloqueada");
                    entrar = 0;
                    while(memoria->lineas_vacias != memoria->memory_size);
                }
                
                    
            }
            
            if (num_linea >= memoria->memory_size)
                num_linea = 0;

            while(strcmp (memoria->lines[num_linea].data, "vacio") == 0){
                
                num_linea = num_linea+1;
                if (num_linea >= memoria->memory_size)
                    num_linea = 0;
                
                    
            }
            
        }while(!entrar);
        memoria->contador_egoista = 0;
        memoria->reader.procesos[PID].linea = num_linea;
        num_linea = num_linea+1;
        semaphore_signal(sem_reader);

        
        
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

        printf("\nlectura en linea %i\n", memoria->reader.procesos[PID].linea);
        printf("proceso %i leyendo\n", PID);

        
        for (int i = 0; i<strlen(memoria->lines[memoria->reader.procesos[PID].linea].data); i++)
		{
			mensaje[i]=memoria->lines[memoria->reader.procesos[PID].linea].data[i];
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
 

        
        fflush(stdout);
        sleep(memoria->reader.execution_time);

        pthread_mutex_unlock(&mutex_archivo);
        if(pthread_mutex_trylock(&mutex_archivo)){
            entrar=0;
            printf("memoria DESbloqueada");
        }


        
        memoria->reader.procesos[PID].status = 0;
        
        printf("proceso %i durmiendo\n", PID);
        sleep(memoria->reader.sleep_time);      

    }
            
    return NULL;
}


int main(int argc, char** argv) {

    int cantidad_readers = atoi(argv[1]);
    int tiempo_sleep= atoi(argv[2]);
    int tiempo_read = atoi(argv[3]);

    pthread_t tid[cantidad_readers];
	
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
    memoria->reader.cant_hijos = cantidad_readers;
    memoria->reader.sleep_time = tiempo_sleep;
    memoria->reader.execution_time = tiempo_read;


    attr_mutex_archivo = memoria->attr_mutex_archivo;
    mutex_archivo = memoria->mutex_archivo;

    pthread_mutexattr_init(&attr_mutex_archivo);
    pthread_mutex_init(&mutex_archivo, &attr_mutex_archivo);

    mach_port_t self = mach_task_self();
    semaphore_create(self, &sem_reader, SYNC_POLICY_FIFO, 1);
    //sem_reader = memoria->sem_reader;
    
    
    
    
    int i;

    for(i = 0; i<cantidad_readers; i++){
        memoria->reader.procesos[i].PID = i;
        pthread_create(&tid[i], NULL, (void*)reader_function, &i);
        sleep(1);
    }

    for(i=0; i<cantidad_readers; i++)
    {
        pthread_join(tid[i], NULL);
    }
    semaphore_destroy(self, sem_reader);

    return 0;
}




void escribir_bitacora(char *msj){
    FILE *bitacora;
    bitacora = fopen ("bitacora.txt", "a");  
    fprintf(bitacora,"Reader-> %s\n",msj);
    fclose(bitacora);

}
//ipcrm -M 6002