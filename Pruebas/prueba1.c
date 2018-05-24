// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/ipc.h>
#include <sys/shm.h> 

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>


#define SHSIZE 200

int main(int argc, char *argv[])
{
    int shmid;
    key_t key;
    char *shm;
    char *s;

    key = 9876;

    shmid = shmget(key, SHSIZE, IPC_CREAT | 0666);
    if(shmid < 0)
    {
        perror("shmget");
        exit(-1);
    }

    shm = shmat(shmid, NULL, 0);

    if(shm == (char *) -1)
    {
        perror("shmat");
        exit(-1);
    }

    memcpy(shm, "Hello Worldd",1024);
    memcpy(shm, "Hello Worlaaaaaaaa",1024);

    s = shm;
    s += 1024;

    *s = 0;

    while(*shm != '*'){
        sleep(1);
    }


    return 0;
}