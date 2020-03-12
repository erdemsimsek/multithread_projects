#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_CHARACTER_PER_THREAD     (100000u)

void getFilePositionForThread(FILE const * fp, unsigned int const * pArray);
unsigned int getNumberOfThread(unsigned int numChar);
unsigned int getNumberOfCharacter(FILE const * const fp);

int main() {
    unsigned int * pFilePosition = NULL;
    unsigned int numberOfCharacter = 0, numberOfThread = 0;
    FILE * fhandle = fopen("../sample_file.txt", "r");

    numberOfCharacter = getNumberOfCharacter(fhandle);
    numberOfThread = getNumberOfThread(numberOfCharacter);

    pFilePosition = malloc(sizeof(unsigned int));

    printf("Number of lines %d, Number of thread %d", numberOfCharacter, numberOfThread);


    return 0;
}


void getFilePositionForThread(FILE const * fp, unsigned int const * pArray){
    /* If the following character is not space, the pointer in the middle of word. Try to cover the word */
    if((fp != NULL) && (pArray != NULL)){

    }

    return;
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