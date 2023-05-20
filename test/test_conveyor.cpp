
#include <shmem.h>
#include <stdio.h>

struct item {
    int value;
    int senders_number;
};



// int main() {
//     shmem_init();
//     convey_t* conveyor = convey_new(SIZE_MAX, 0, NULL, 0);
//     if(!conveyor){printf("ERROR: histo_conveyor: convey_new failed!\n"); return(-1.0);}
//     int ret = convey_begin(conveyor, sizeof(int64_t), 0);
//     if(ret < 0){printf("ERROR: histo_conveyor: begin failed!\n"); return(-1.0);}

//     printf("Finished inititialization\n");
//     int num = 4000, i=0, pe = (shmem_my_pe() + 1)%shmem_n_pes();
//     while(convey_advance(conveyor, i == num)) {
//         for(; i< num; i++){
//             int64_t val = shmem_my_pe() * 1000 + i;
//             if( !convey_push(conveyor, &val, pe)) break;
//         }
//         int64_t pop;
//         while( convey_pull(conveyor, &pop, NULL) == convey_OK) {
//             printf("In rank %d, val %d\n", shmem_my_pe(), pop);
//         }
//     }
//     convey_free(conveyor);
//     shmem_finalize();
//     return 0;
// }

int main() {
    shmem_init();
    struct item *i = (struct item *) malloc(sizeof(item));
    struct item *ii = (struct item *) malloc(sizeof(item));
    ii->value = shmem_my_pe() + 1000;
    ii-> senders_number = shmem_my_pe();
    int remote_pe;
    if(shmem_my_pe() == 1) {
        remote_pe = 0;
    }
    else {
        remote_pe = 1;
    }
    struct item *shm_ptr = shmem_ptr((void *) i, remote_pe);
    memcpy(shm_ptr, ii, sizeof(item));
    fprintf(stderr, "my_pe(): %d, Item generated: %d, %d\n", shmem_my_pe(), ii->value, ii->senders_number);
    fprintf(stderr, "my_pe(): %d, Item received: %d, %d\n", shmem_my_pe(), shm_ptr->value, shm_ptr->senders_number);
    shmem_finalize();
    return 0;
}
