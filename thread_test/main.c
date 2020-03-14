#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int thRet1, thRet2;
pthread_t th1, th2;
int param1 = 1, param2 = 2;

static int index = 0;

void incrementCounter(){
    index++;
}


void * printFunction(void * pParams){
    int val = *(int *)pParams;

    pthread_cancel(th2);

    incrementCounter();
    printf("This is thread %d. Index number is %d\n",val, index);
}

void * waitInfinite(void * pParams){
    while (1);
}

int main() {
    thRet1 = pthread_create(&th1, NULL, printFunction, (void *)&param1);
    thRet2 = pthread_create(&th2, NULL, printFunction, (void *)&param2);

    //pthread_join(th1, NULL);
    //pthread_join(th2, NULL);

    while (1);
    return 0;
}