#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <stdbool.h>


// global set up
 mtx_t lk;
 int total;

 int count(void *arg){
    (void)arg;
    bool gotLock = false;
    while(!gotLock){
        mtx_lock(&lk);
            total ++;
            gotLock = true;
        mtx_unlock(&lk);
    }
    return 0;
}

int main(int argc, char **argv) {
    mtx_init(&lk, mtx_plain);
    int thread_count = 100;
    total = 0;

    thrd_t threads[thread_count];

    for (size_t i = 0; i < thread_count; i++) thrd_create(threads + i, count, NULL);

    for (size_t i = 0; i < thread_count; i++) thrd_join(threads[i], NULL);
    printf("%d\n", total);
    mtx_destroy(&lk);
    return 0;
}
