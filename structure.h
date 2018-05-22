#include <semaphore.h>

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