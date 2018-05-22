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
	char data[30];

}Line;

typedef struct{
	int PID;
	int status; // 0->dormido  1->wait  2->ejecucion

}Hijo;

typedef struct{
	int type;  //1-> reader  2->writer  3->egoista
	int sleep_time;
	int execution_time;
	Hijo procesos[];

}Padre;

typedef struct{
	sem_t sem_writer;
	sem_t sem_reader;
	sem_t sem_egoista;
	Padre writer;
	Padre reader;
	Padre egoista;
	int PID_count;
	Line line[];
	int memory_size;
	int finalizar; //bool que mantendra todos los ciclos activos hasta que se finalice el programa

}Programa;

/*
	Estructuras:
	-Padre(tipo,guarda tiempo de escritura/lectura, sleep)
	-Hijo(PID, bool de: dormido, wait, ejecucion)
	-Programa:(semaforos(writer, reader, egoista), padres,PID_COUNT,linea[])
	-Linea(id, data)
 */