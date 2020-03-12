#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#define NUM_CHARACTER_PER_THREAD     (100000u)

typedef struct{
    char fileName[48];
    char searchText[48];
} searchParameters;

unsigned int getThreadIndex();
void * getStartToSearch(searchParameters const * pParams);
void getFilePositionForThread(FILE const * fp, unsigned int * const pArray, unsigned int numThread);
unsigned int getNumberOfThread(unsigned int numChar);
unsigned int getNumberOfCharacter(FILE const * const fp);

static int g_threadIndex = -1;
static pthread_mutex_t g_threadIndexLock;
static unsigned int * pFilePosition = NULL;

int main() {

    unsigned int numberOfCharacter = 0, numberOfThread = 0;
    int * pThreadReturn = NULL;
    pthread_t * pThreadArr = NULL;
    FILE * fhandle;
    searchParameters searchParams;

    pthread_mutex_init(&g_threadIndexLock, NULL);

    memcpy(searchParams.fileName, "../sample_file.txt", 18);
    memcpy(searchParams.searchText, "entertaining", 12);

    fhandle = fopen("../sample_file.txt", "r");
    numberOfCharacter = getNumberOfCharacter(fhandle);
    numberOfThread = getNumberOfThread(numberOfCharacter);

    pFilePosition = malloc(sizeof(unsigned int));
    pThreadArr = malloc(sizeof(pthread_t));
    pThreadReturn = malloc(sizeof(int));

    getFilePositionForThread(fhandle,pFilePosition,numberOfThread);
    fclose(fhandle);

    printf("Number of lines %d, Number of thread %d", numberOfCharacter, numberOfThread);

    size_t threadIndex = 0;

    for(threadIndex = 0; threadIndex < numberOfThread; threadIndex++){
        pThreadReturn[threadIndex] = pthread_create(&pThreadArr[threadIndex], NULL, getStartToSearch, (void *)&searchParams);
    }

    for(threadIndex = 0; threadIndex < numberOfThread; threadIndex++){
        pthread_join(pThreadArr[threadIndex], NULL);
    }

    return 0;
}

void * getStartToSearch(searchParameters const * pParams){
    int threadIndex;
    size_t index = 0;
    bool isFound = false;

    FILE * file;
    file = fopen("../sample_file.txt", "r");


    if(file != NULL){
        threadIndex = getThreadIndex();
        fseek(file, pFilePosition[threadIndex], SEEK_SET);
        while(ftell(file) < (pFilePosition[threadIndex + 1])){
            for(index = 0; index < (strlen(pParams->searchText) - 1); index++){
                if(fgetc(file) != pParams->searchText[index]){
                    isFound = false;
                }
                else{
                    isFound = true;
                }
            }
            if(isFound){
                printf("%d\n", ftell(file));
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