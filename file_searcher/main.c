#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>

#define NUM_CHARACTER_PER_THREAD     (1000000u)
#define QUEUE_NAME  "test_queue"

typedef struct{
    char fileName[48];
    char searchText[48];
} searchParameters;

unsigned int getThreadIndex();
void * getStartToSearch(searchParameters const * pParams);
void * displayText();
void sendToQueue(unsigned int index);
void getFilePositionForThread(FILE const * fp, unsigned int * const pArray, unsigned int numThread);
unsigned int getNumberOfThread(unsigned int numChar);
unsigned int getNumberOfCharacter(FILE const * const fp);

static int g_threadIndex = -1;
static pthread_mutex_t g_threadIndexLock;
static unsigned int * pFilePosition = NULL;
static mqd_t g_msg_queue;

int main() {

    unsigned int numberOfCharacter = 0, numberOfThread = 0;
    int * pThreadReturn = NULL;
    pthread_t * pThreadArr = NULL;

    int openFlags = 0;
    int displayThread;
    pthread_t displayThreadArr;

    FILE * fhandle;
    searchParameters searchParams;
    struct mq_attr attr;

    pthread_mutex_init(&g_threadIndexLock, NULL);

    openFlags = O_CREAT | O_RDWR;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize = sizeof(unsigned int);
    attr.mq_flags = 0;
    g_msg_queue = mq_open("myqueue",openFlags,&attr);

    if(g_msg_queue == (-1)){
        printf("mq_open failure %d\n", errno);
    }

    memcpy(searchParams.fileName, "../sample_file.txt", 18);
    memcpy(searchParams.searchText, "entertaining", 12);

    fhandle = fopen("../sample_file.txt", "r");
    numberOfCharacter = getNumberOfCharacter(fhandle);
    numberOfThread = getNumberOfThread(numberOfCharacter);

    printf("Number of character %d \n", numberOfCharacter);
    printf("Number of thread %d \n", numberOfThread);

    pFilePosition = malloc(sizeof(unsigned int));
    pThreadArr = malloc(sizeof(pthread_t));
    pThreadReturn = malloc(sizeof(int));

    getFilePositionForThread(fhandle,pFilePosition,numberOfThread);
    fclose(fhandle);

    size_t threadIndex = 0;
    for(threadIndex = 0; threadIndex < numberOfThread; threadIndex++){
        pThreadReturn[threadIndex] = pthread_create(&pThreadArr[threadIndex], NULL, getStartToSearch, (void *)&searchParams);
    }

    for(threadIndex = 0; threadIndex < numberOfThread; threadIndex++){
        pthread_join(pThreadArr[threadIndex], NULL);
    }

    displayThread = pthread_create(&displayThreadArr, NULL, displayText, NULL);
    pthread_join(&displayThreadArr, NULL);

    mq_unlink("myqueue");
    mq_close(g_msg_queue);

    return 0;
}

void *displayText(){
    int received = 1;

    while(received > 0){
        received = mq_receive(g_msg_queue, &index, sizeof(unsigned int), 0);

        if(received == (-1)){
            printf("queue receive error %d\n", errno);
        }
        else{
            printf("Index %d\n", index);
        }
    }


}

void * getStartToSearch(searchParameters const * pParams){
    int threadIndex;
    unsigned int basePoint, endPoint;
    size_t index = 0;
    bool isFound = false;
    unsigned int as;
    FILE * file;
    file = fopen("../sample_file.txt", "r");

    if(file != NULL){
        threadIndex = getThreadIndex();
        basePoint = pFilePosition[threadIndex];
        endPoint = pFilePosition[threadIndex + 1];

        if(endPoint < basePoint){
            fseek(file, 0, SEEK_END);
            endPoint = ftell(file);
        }

        fseek(file, basePoint, SEEK_SET);
        while(ftell(file) < endPoint){
            for(index = 0; index < (strlen(pParams->searchText) - 1); index++){

                if(fgetc(file) == pParams->searchText[index]){
                    isFound = true;
                }
                else{
                    isFound = false;
                    break;
                }
            }
            if(isFound){
                as = ftell(file);
                sendToQueue(as);
                //printf("%d\n", ftell(file));
                isFound = false;
            }
        }
        fclose(file);
    }

}

unsigned int getThreadIndex(){
    pthread_mutex_lock(&g_threadIndexLock);
    g_threadIndex++;
    pthread_mutex_unlock(&g_threadIndexLock);

    return g_threadIndex;
}

void getFilePositionForThread(FILE const * fp, unsigned int * const pArray, unsigned int numThread){
    size_t index = 0;
    unsigned int pos = 0;
    /* If the following character is not space, the pointer in the middle of word. Try to cover the word */
    if((fp != NULL) && (pArray != NULL)){
        for(index = 0; index < numThread; index++){
            pos = (index * NUM_CHARACTER_PER_THREAD);
            fseek(fp, pos, SEEK_SET);
            while(fgetc(fp) != ' '){
                pos++;
            }
            pArray[index] = pos;
        }
        /* It is a special case */
        pArray[0] = 0;
    }
}

unsigned int getNumberOfThread(unsigned int numChar){
    if((numChar % NUM_CHARACTER_PER_THREAD) == 0){
        return (numChar / NUM_CHARACTER_PER_THREAD);
    }
    else{
        return ((numChar / NUM_CHARACTER_PER_THREAD) + 1);
    }
}

unsigned int getNumberOfCharacter(FILE const * const fp){
    unsigned int numChar = 0;

    if(NULL != fp){
        fseek(fp, 0, SEEK_END);
        numChar = ftell(fp);
    }
    return numChar;
}

void sendToQueue(unsigned int index){
    int status = 0;

    status = mq_send(g_msg_queue, index, sizeof(unsigned int), 1);
    if(status == (-1)){
        printf("queue send fail %d\n",errno);
    }
    else{
        printf("queue send success\n");
    }
}