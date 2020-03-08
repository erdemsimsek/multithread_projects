#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef char ALIGN[16];

union header{
    struct {
        size_t size;
        unsigned is_free;
        union header * next;
    }s;
    ALIGN stub;
};

typedef union header header_t;

/* Function Declaration */
void * malloc(size_t size);
void * realloc(void * block, size_t size);
void free(void * block);
static header_t * get_free_block(size_t size);

static header_t * head, * tail;
static pthread_mutex_t mutex_malloc_block;

int main() {
    char * ptr;
    int size;

    printf("Please, Enter the size of text: ");
    scanf("%d", &size);

    ptr = malloc(size);
    if(ptr){
        printf("Memory allocation successful");
    }
    else{
        printf("Memory allocation failed");
    }

    free(ptr);

    return 0;
}


void * malloc(size_t size){
    size_t total_size;
    void * block;
    header_t * header;

    if(!size)
        return NULL;

    pthread_mutex_lock(&mutex_malloc_block);
    header = get_free_block(size);

    if(header){
        header->s.is_free = 0;
        pthread_mutex_unlock(&mutex_malloc_block);
        return (void *)(header + 1);
    }

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if(block == (void *) -1){
        pthread_mutex_unlock(&mutex_malloc_block);
        return NULL;
    }

    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;

    if(!head){
        head = header;
    }

    if(tail){
       tail->s.next = header;
    }

    tail = header;
    pthread_mutex_unlock(&mutex_malloc_block);

    return (void *)(header + 1);
}

void free(void * block){
    header_t * header, * temp;
    void * programBreak;

    if(!block){
        return;
    }

    pthread_mutex_lock(&mutex_malloc_block);
    header = (header_t *)block -1 ;

    programBreak = sbrk(0);

    /* If the program is at the end of heap */
    if((char *) block + header->s.size == programBreak){
        if(head == tail){
            head = tail = NULL;
        } else{
            temp = head;
            while (temp){
                if(temp->s.next == tail){
                    temp->s.next = NULL;
                    tail = temp;
                }
                temp = temp->s.next;
            }
        }

        sbrk(0 - sizeof(header_t) - header->s.size);
        pthread_mutex_unlock(&mutex_malloc_block);
        return;
    }
    /* If the block is in the middle of heap, flag it as free */
    header->s.is_free = 1;
    pthread_mutex_unlock(&mutex_malloc_block);
    return;
}

void * realloc(void * block, size_t size){
    header_t * header;
    void * ret;

    if(!block || !size){
        return malloc(size);
    }

    header = (header_t *)(block - 1);
    if(header->s.size >= size){
        return block;
    }

    ret = malloc(size);
    if(ret){
        memcpy(ret, block, header->s.size);
        free(block);
    }

    return ret;
}

/* Check is there an available space for that */
static header_t * get_free_block(size_t size){
    header_t * curr = head;
    while(curr){
        if (curr->s.is_free && curr->s.size >= size){
            return curr;
        }
        curr = curr->s.next;
    }
    return NULL;
}