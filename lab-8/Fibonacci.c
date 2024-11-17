#include <stdlib.h>
#include <stdio.h>
#include <threads.h>
#include <stdbool.h>

// set up global var
mtx_t lk;
cnd_t cv;
int *fibNumbers;
int fibCount = 2;
bool flag = false;
int numOfFib;


int fib_parent(void *arg){
    //prints out fib sequence
    (void)arg;
    //num of fib numbers printed out
    int count = 0;

    //if run until all the numbers are printed
    while(count < numOfFib){
        //if child thread is still running try wait sequence else no need for the lock
        if(fibCount < numOfFib){
            do{
                mtx_lock(&lk);
                cnd_wait(&cv, &lk);
            }while(!flag);
        }
        //print numbers that have been calculated so far
        while(count < fibCount){
            printf("%d\n", fibNumbers[count]);
            count ++;
        }
        //once out of numbers reset loop and give up lock if locked
        flag = false;
        mtx_unlock(&lk);
    }
    return 0;
}



int fib_child(void *arg){
    (void)arg;
    //set up first two fib numbers
    fibNumbers[0] = 1;
    fibNumbers[1] = 1;
    //while their are numbers left to generate keep running
    while(fibCount < numOfFib){
        mtx_lock(&lk);
        fibNumbers[fibCount] = fibNumbers[fibCount - 1] + fibNumbers[fibCount - 2];
        flag = true;
        fibCount ++;
        //signal that a number has been created
        cnd_signal(&cv);
        mtx_unlock(&lk);
    }
    //signal that the loop has ended. Not sure if this is necessary
    flag = true;
    cnd_signal(&cv);
    return 0;
}


int main(int argc, char **argv) {
    mtx_init(&lk, mtx_plain);
    cnd_init(&cv);
    //add one to go up to the said number in the array
    numOfFib = atoi(argv[argc - 1]) + 1;
    //checks for base cases
    if ((numOfFib - 1) < 2){
        for (int i = 0; i < numOfFib; i++) printf("1\n");
        goto end;
    }
    fibNumbers = malloc(numOfFib * sizeof(int));
    //if malloc fails
    if (!fibNumbers){
        goto end;
    }

    //tread creation
    thrd_t threads[2];

    thrd_create(threads, fib_child, NULL);
    thrd_create(threads + 1, fib_parent, NULL);

    for (size_t i = 0; i < 2; i++) thrd_join(threads[i], NULL);

    //freeing mem
    free(fibNumbers);
    end:
    cnd_destroy(&cv);
    mtx_destroy(&lk);
    return 0;
}