#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/times.h>

#include <signal.h>
#define NTHREADS 10

volatile int should_terminate = 0;

void hello(int signum) {
    printf("Hello Signal !\n");
}

pthread_t threads[NTHREADS];

void *thread_fun(void *num) {
    //If not a malloc, local allocation for *int and segfault when receiving in join later !!
    int *i_ptr = (int *)malloc(sizeof(int));

    int i = *(int *)num;

    *(i_ptr) = i + 20;

    if (i == 7){//Testing inter thread communication with signals
        sigset_t set;
        int sig;

        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        pthread_sigmask(SIG_BLOCK, &set, NULL);
        sigwait(&set, &sig);

        printf("Signal received\n");

    }

    if (i == 2){ // Testing inter thread communication with volatile
        while (!should_terminate){
            //do nothing
        }
    }

    if (i == 4){
        should_terminate = 1;
    }

    if(i == 9){
        pthread_kill(threads[7], SIGUSR1);
    }

    printf("Thread %d\n", i); // Or pthread_self()

    pthread_exit(i_ptr); // Or simply return NULL
}


int main(int argc, char *argv[]) {
    pthread_attr_t attr;
    int i, error;

    //We launch the threads here, with a default routine

    for (i = 0; i < NTHREADS; i++) {
        pthread_attr_init(&attr); //Default thread attributes
        int *ii = (int *)malloc(sizeof(int)); //Portable malloc casted to pointer to int... b
        *ii = i; //we put in ii the value i, and it will be send as args
        error = pthread_create(&threads[i], &attr, thread_fun, ii);
        // printf("%lu\n", (unsigned long) threads[i] );
         //Return the error value yes
        if (error != 0) {
            fprintf(stderr, "Error in pthread_create: %s \n", strerror(error));
            exit(1);
        }
    }

    int *val = (int *)malloc(sizeof(int));
    int **retval = &val;
    // We join the threads and try to retrieve their return value in the main one
    for (i=0; i < NTHREADS; i++) {
        
        error = pthread_join(threads[i], (void **)retval);
        printf("Retval Join %d\n", **retval);

        if (error != 0) {
            fprintf(stderr, "Error in pthread_join: %s \n", strerror(error));
            exit(1);
        }
    }

}