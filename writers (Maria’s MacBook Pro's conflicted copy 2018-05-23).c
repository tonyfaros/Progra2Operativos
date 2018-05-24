
#include <iso646.h>

#include "writer_structure.h"
char *line = "---------------------------------------------------\n";

void *writer_function(void *vargp);
void escribir_bitacora(char *msj);

/*
 * WRITER
 */
int main(int argc, char** argv) {

    int cant_writers = atoi(argv[1]);
    int tiempo_sleep = atoi(argv[2]);
    int tiempo_ejecucion = atoi(argv[3]);
	//sprintf(total_lines, "%d", parametro);

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
             
    return 0;
}

void *writer_function(void *vargp)
{     
    Writer *w = (Writer*) vargp;
    time_t ltime;
    struct tm *tm;         
    char *shm = (char*) shmat(w->shmid,(void*)0,0);       
    int pages = strlen(shm)/25;
    int len = strlen(shm);
    int regBase = 0;
    int cont = 0;
    int i = 0;
    printf("len: %d \n",len);
    printf("memoria %s \n",shm);
    while(i <= 6){
        regBase = cont*25;                                                         
        if(shm[regBase]=='w'){
            printf("Casilla llena en el registro Base:%d,contenido:%s,con id:%d\n",regBase,&shm[regBase],w->id); 
            printf("%s",line);
        }else{
            sleep(w->tiempo_write);
            
            char *timestamp = (char *)malloc(sizeof(char) * 16);  
            ltime=time(NULL);    
            tm=localtime(&ltime); 
            sprintf(timestamp,"%s%d-%04d/%02d/%02d-%02d:%02d:%02d","w",w->id, tm->tm_year+1900, tm->tm_mon, 
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);                
            int len_stamp = strlen(timestamp);
            int j;
            for(j=0;j<25;j++){
                if(j < len_stamp){
                    char ct = timestamp[j];
                    shm[j+regBase] = ct;
                }else{                    
                    shm[j+regBase] = '-';
                }                                               
            }
            //escribir_bitacora(timestamp);
            printf("\nSe escribio en el registro Base:%d,%s, con id:%d\n",regBase,shm,w->id);                                       
            printf("%s",line);
            sleep(w->tiempo_sleep);
        }
        
        cont++;
        i=i+1;
        if(cont == pages){
            cont = 0;
        }
    }      
            
    return NULL;
}

void escribir_bitacora(char *msj){
    FILE *bitacora;
    bitacora = fopen ("bitacora.txt", "a+");  
    fprintf(bitacora,"Writer-> %s\n",msj);
    fclose(bitacora);
}