
#define WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

typedef struct{
	int id;
	int shmid;
	int sleep_time;
	int writing_time;
	

}Writer;