#include <semaphore.h>
/*
typedef struct{
	int Id;
	char data[30];
	int line;

}Line;


typedef struct{
	sem_t sem_writer;
	sem_t sem_reader;
	sem_t sem_spy;
	sem_t sem_end_writer;
	sem_t sem_end_reader;
	sem_t sem_end_spy;
	char writer_write;
	char reader_read;
	char spy_read;
	Line lines[];
}Shared_mem;
*/


typedef struct{
	int Id;
	char data[100];

}Line;

typedef struct Hijo{
	int PID;
	int status; // 0->dormido  1->wait  2->ejecucion
	int linea;

}hijo;;

typedef struct Padre{
	//int type;  //1-> reader  2->writer  3->egoista
	int sleep_time;
	int execution_time;
	int cant_hijos;
	hijo procesos[100];

}padre;
typedef struct Programa{
	pthread_mutex_t *mutex_archivo;
	pthread_mutexattr_t *attr_mutex_archivo;
	sem_t * sem_writer;
	sem_t * sem_reader;
	sem_t * sem_egoista;
	padre writer;
	padre reader;
	padre egoista;
	Line lines[1000]; 
	int memory_size;
	int lineas_vacias;
	int contador_egoista; //cuando llega a 3 se bloquean
	int finalizar; //bool que mantendra todos los ciclos activos hasta que se finalice el programa

}programa;
 

/*
	Estructuras:
	-Padre(tipo,guarda tiempo de escritura/lectura, sleep)
	-Hijo(PID, bool de: dormido, wait, ejecucion)
	-Programa:(semaforos(writer, reader, egoista), padres,PID_COUNT,linea[])
	-Linea(id, data)
 */