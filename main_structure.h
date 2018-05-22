#define CONTROLADOR_H

int pedir_sem_procs(Mem_comp *mem);
tipo_proc selec_segun_jerar(Mem_comp *mem, char *contador_r_e);
tipo_proc selec_segun_3_r_e(Mem_comp *mem, char *contador_r_e);
void spawn_fin_chck_thread(Mem_comp *mem, tipo_proc tipo_selec);
void relock_sem_shm(void *tmp);

typedef enum {
    writer,
    reader,
    r_e,
    espia
} tipo_proc;

struct tmp_data {
    tipo_proc tipo;
    Mem_comp *mem;
};