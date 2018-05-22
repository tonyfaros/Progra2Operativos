#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "structure.h"
#include "controlador.h"

/*
 *  Implementa las reglas especiales de coordinación entre procesos.
 *  Prioridades entre procesos:
 *      1) Writer / Reader egoísta
 *      2) Reader
 *  Monitorea situación 3 Readers egoístas seguidos:
 *      Tienen prioridad sobre los Reader normales, pero misma que los Writer.
 *          No pueden acceder 3 de ellos consecutivamente, entonces se le
 *      entrega a un proceso de otro tipo. En este escenario, los Writer tienen
 *      prioridad sobre los Reader. Si no hay nadie más, pueden continuar hasta
 *      que llegue alguien más.
 */
int main(int argc, char** argv) {
    key_t key = ftok("shmfile", 21);
    int shmid = shmget(key, 1, 0666 | IPC_CREAT);
    Mem_comp *mem = shmat(shmid, NULL, 0);

    pedir_sem_procs(mem);

    srand(time(NULL));
    char contador_r_e = 0;
    tipo_proc tipo_selec;
    while (1) {
        if (mem->r_e_wants_shm && contador_r_e == 2)
            tipo_selec = selec_segun_3_r_e(mem);
        else
            tipo_selec = selec_segun_jerar(mem, &contador_r_e);

        if (tipo_selec == -1)
            spawn_fin_chck_thread(mem, tipo_selec);
    }

    return (EXIT_SUCCESS);
}

/*
 * Bloquea todos los procesos para que dependan de la coordinación 
 * del Controlador.
 */
void pedir_sem_procs(Mem_comp *mem) {
    sem_wait(&mem->sem_shm_writer);
    sem_wait(&mem->sem_shm_reader);
    sem_wait(&mem->sem_shm_r_e);
    sem_wait(&mem->sem_shm_espia);
}

/*
 * Implementa la vigilancia de la situación de excepción que prohíbe el acceso
 * seguido de 3 Readers egoístas.
 */
tipo_proc selec_segun_3_r_e(Mem_comp* mem, char *contador_r_e) {
    int random;
    tipo_proc tipo = -1;
    if (mem->writer_wants_shm) {
        sem_post(&(mem->sem_shm_writer));
        *contador_r_e = 0;
        return writer;
    } else { // (EDITAR si cambia la de abajo, son lo mismo en lógica)
        if (mem->reader_wants_shm || mem->espia_wants_shm) {
            if (mem->reader_wants_shm && mem->espia_wants_shm) {
                random = rand();
                random > 0 ? random = 1 : random = 0;
                if (random) {
                    sem_post(&(mem->sem_shm_reader));
                    tipo = reader;
                } else {
                    sem_post(&(mem->sem_shm_espia));
                    tipo = espia;
                }
            } else {
                if (mem->reader_wants_shm) {
                    sem_post(&(mem->sem_shm_reader));
                    tipo = reader;
                }
                else {
                    sem_post(&(mem->sem_shm_espia));
                    tipo = espia;
                }
            }

            *contador_r_e = 0;
        }
    }

    if (mem->r_e_wants_shm) {
        sem_post(&(mem->sem_shm_r_e));
        return r_e;
    }
    
    return tipo;
}

/*
 * Implementa la jerarquía entre procesos que determina cuál entra a la región
 * crítica.
 */
tipo_proc selec_segun_jerar(Mem_comp *mem, char *contador_r_e) {
    int random;
    tipo_proc tipo = -1;
    if (mem->writer_wants_shm || mem->r_e_wants_shm) {
        if (mem->writer_wants_shm && mem->r_e_wants_shm) { // Ambos pidieron
            random = rand();
            random > 0 ? random = 1 : random = 0;
            if (random) {
                sem_post(&(mem->sem_shm_writer));
                *contador_r_e = 0;
                tipo = writer;
            } else {
                sem_post(&(mem->sem_shm_r_e));
                *contador_r_e++;
                tipo = r_e;
            }
        } else {
            if (mem->writer_wants_shm) { // Para averiguar cuál fue
                sem_post(&(mem->sem_shm_writer));
                *contador_r_e = 0;
                tipo = writer;
            } else {
                sem_post(&(mem->sem_shm_r_e));
                *contador_r_e++;
                tipo = r_e;
            }
        }
    } else {
        if (mem->reader_wants_shm || mem->espia_wants_shm) {
            if (mem->reader_wants_shm && mem->espia_wants_shm) {
                random = rand();
                random > 0 ? random = 1 : random = 0;
                if (random) {
                    sem_post(&(mem->sem_shm_reader));
                    tipo = reader;
                } else {
                    sem_post(&(mem->sem_shm_espia));
                    tipo = espia;
                }
            } else {
                if (mem->reader_wants_shm) {
                    sem_post(&(mem->sem_shm_reader));
                    tipo = reader;
                }
                else {
                    sem_post(&(mem->sem_shm_espia));
                    tipo = espia;
                }
            }

            *contador_r_e = 0;
        }
    }
    
    return tipo;
}

/*
 * Levanta un hilo que espera a que el proceso entrante a la región crítica
 * finalice para volver a capturar su semáforo para bloquerlo nuevamente.
 */
void spawn_fin_chck_thread(Mem_comp *mem, tipo_proc tipo_selec) {
    pthread_t fin_chck_thread;
    struct tmp_data data = {tipo_selec, mem};
    pthread_create(fin_chck_thread, NULL, relock_sem_shm, (void*) data);
}

/*
 * Selección como tal del semáforo que corresponde tanto para notificar
 * finalización por parte de un proceso como del que lo bloquea para entrar
 * a la región crítica.
 */
void relock_sem_shm(void *tmp) {
    struct tmp_data data = (struct tmp_data) tmp;
    switch (data.tipo) {
        case writer:
            sem_wait(&(data.mem->sem_fin_writer));
            sem_wait(&(data.mem->sem_shm_writer));
            break;

        case reader:
            sem_wait(&(data.mem->sem_fin_reader));
            sem_wait(&(data.mem->sem_shm_reader));
            break;

        case r_e:
            sem_wait(&(data.mem->sem_fin_r_e));
            sem_wait(&(data.mem->sem_shm_r_e));
            break;

        case espia:
            sem_wait(&(data.mem->sem_fin_espia));
            sem_wait(&(data.mem->sem_shm_espia));
            break;
    }
}