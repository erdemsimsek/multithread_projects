#include <stdio.h>
#include <stdint.h>

struct mem_pool{
    uint32_t maximum_block_size;
    uint32_t minimum_block_size;
    uint32_t number_of_layer;
    void * p_buff;
};

static struct mem_pool memory;

int getPowerByTwo(int number);
void createMemoryPool(uint32_t max_block_size, uint32_t min_block_size);

int main() {
    createMemoryPool(256,32);
}

int getPowerByTwo(int number){
    int counter = 0;

    while(number != 1){
        number = number >> 1;
        counter++;
    }
    return counter;
}

void createMemoryPool(uint32_t max_block_size, uint32_t min_block_size){
    
}