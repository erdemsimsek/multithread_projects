#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

int thRet1, thRet2, thRet3;
pthread_t th1, th2, th3;
pthread_mutex_t  lock;
int param1 = 1, param2 = 2, param3 = 3;

static sem_t sem;

static long int index = 0;

void incrementCounter(){
    index++;
}

void decrementCounter(){
    index--;
}

void * printFunction(void * pParams){
    int val = *(int *)pParams;
    size_t ind = 0;
    int sem_val;

    //pthread_mutex_lock(&lock);
    sem_wait(&sem);
    sem_getvalue(&sem, &sem_val);
    printf("Semaphore values is %d \n", sem_val);
    for(ind = 0; ind < 1000; ind++){
        if(val == 1){
            incrementCounter();
        }
        else{
            decrementCounter();
        }
        //printf("This is thread %d. \n", val);
    }

    printf("This is thread %d. Index number is %d\n",val, index);
    //sem_post(&sem);
    //pthread_mutex_unlock(&lock);
}

void * waitInfinite(void * pParams){
    while (1);
}

int main() {

    if(sem_init(&sem, 0, 3) == 0){
        printf("semaphore creation successful\n");
    }

    if(pthread_mutex_init(&lock, NULL) == 0){
        printf("mutex creation successful\n");
    }

    thRet1 = pthread_create(&th1, NULL, printFunction, (void *)&param1);
    thRet2 = pthread_create(&th2, NULL, printFunction, (void *)&param2);
    thRet3 = pthread_create(&th3, NULL, printFunction, (void *)&param3);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_join(th3, NULL);

    //while (1);

    pthread_mutex_destroy(&lock);

    return 0;
}