

#include "reader_structure.h"

void *reader_function(void *vargp);
void escribir_bitacora(char *msj);
char *line = "\n---------------------------------------------------\n";
/*
 * READER
 */
int main(int argc, char** argv) {
    int n_procesos = 1;
    int t_sleep = 4;
    int t_read = 2;    
    key_t key = ftok("shmfile",21);        
    int shmid = shmget(key,1,0666|IPC_CREAT);  
    pthread_t readr_array[4];    
    int i = 0;        
    while(i<n_procesos){    
        Reader *reader = malloc(sizeof(Reader));        
        reader->id = i;
        reader->shmid = shmid;
        reader->tiempo_sleep = t_sleep;
        reader->tiempo_read = t_read;        
        pthread_create(&readr_array[i], NULL, reader_function, (void*) reader);                                
        i=i+1;
    }
    pthread_join(readr_array[0], NULL); 
                   
    
             
    return (EXIT_SUCCESS);
}

void *reader_function(void *vargp)
{     
    Reader *reader = (Reader*) vargp;
    time_t ltime;
    struct tm *tm;         
    char *shm = (char*) shmat(reader->shmid,(void*)0,0);       
    int pages = strlen(shm)/25;
    int len = strlen(shm);
    int regBase = 0;
    int cont = 0;
    int i = 0;
    while(i <= 6){
        regBase = cont*25;                                                         
        if(shm[regBase]=='0'){
            printf("Casilla vacia \n"); 
            printf("%s",line);
        }else{
            sleep(reader->tiempo_read);
            
            char *timestamp = (char *)malloc(sizeof(char) * 16);  
            ltime=time(NULL);    
            tm=localtime(&ltime); 
            sprintf(timestamp,"%s%d-%04d/%02d/%02d-%02d:%02d:%02d","r",reader->id, tm->tm_year+1900, tm->tm_mon, 
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);                            
            escribir_bitacora(timestamp);
            printf("\nSe lee en el registro Base %d:",regBase);                                       
            int j;
            for(j=0;j<25;j++){
                printf("%c",shm[regBase+j]);
            }            
            printf("%s",line);
            sleep(reader->tiempo_sleep);
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
    bitacora = fopen ("/home/fauricio/NetBeansProjects/Readers – Writers/bitacora.txt", "a+");  
    fprintf(bitacora,"Reader-> %s\n",msj);
    fclose(bitacora);
}