#define READER
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <time.h>

typedef struct{
    int id;
    int shmid;    
    int tiempo_sleep;
    int tiempo_read;
} Reader;