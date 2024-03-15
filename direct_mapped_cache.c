#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define CACHE_SIZE 32  // 64 bytes,16 words,32 characters
#define BLOCK_SIZE 4 // 8 bytes,2 words,4 characters
#define NUM_CACHE_LINES (CACHE_SIZE / BLOCK_SIZE) //8
#define MEMORY_ROWS 8
#define MEMORY_COLS 8 
char memory[MEMORY_ROWS][MEMORY_COLS][BLOCK_SIZE + 1];
struct CacheLine{
    int valid;
    int tag;
    char* data;
} ;

struct CacheLine** initializeCache(struct CacheLine*** cache) {
    (*cache)=malloc(sizeof(struct CacheLine*)* NUM_CACHE_LINES);
    for (int i = 0; i < NUM_CACHE_LINES; i++) {
        (*cache)[i]=malloc(sizeof(struct CacheLine));
        (*cache)[i]->valid = 0;
        (*cache)[i]->tag = -1; 
        (*cache)[i]->data=malloc(sizeof(char)*(BLOCK_SIZE + 1));
        (*cache)[i]->data[0]='\0';
    }
    return *cache;
}

char* readFromMemory(int row, int col) {
    return memory[row][col];
}

char readFromCache(int address,struct CacheLine** cache) {
    int blockAddress = address / (2*BLOCK_SIZE);
    int index = blockAddress % NUM_CACHE_LINES;
    int tag = blockAddress / NUM_CACHE_LINES;
    int offset=address % BLOCK_SIZE;
    printf("%d %d %d %d\n",offset,blockAddress,index,tag);
    if (cache[index]->valid && cache[index]->tag == tag) {
        printf("Cache hit!\n");
        printf("In cache : %s\n",cache[index]->data);
        return cache[index]->data[offset];
    } else {
        printf("Cache miss!\n");
        strcpy(cache[index]->data,readFromMemory(index,tag));
        cache[index]->valid = 1;
        cache[index]->tag = tag;
        printf("In cache : %s\n",cache[index]->data);
        return cache[index]->data[offset];
    }
}

void writeToMemory(int row, int col, char* data) {
     strcpy(memory[row][col],data);
}

void writeToCache(int address, char* data,struct CacheLine** cache) {
    int blockAddress = address / (2*BLOCK_SIZE);
    int index = blockAddress % NUM_CACHE_LINES;
    int tag = blockAddress / NUM_CACHE_LINES;
    printf("%d %d %d\n",blockAddress,index,tag);
    if (cache[index]->valid && cache[index]->tag == tag) {
        printf("Cache hit for write!\n");
        strcpy(cache[index]->data ,data);
        writeToMemory(index, tag, data);
    } else {
        printf("Cache miss for write!\n");
        strcpy(cache[index]->data,readFromMemory(index,tag)); //write allocate
        cache[index]->valid = 1;
        cache[index]->tag = tag;
        writeToMemory(index,tag, data); 
    }
}

void printMemoryContents() {
    printf("Memory contents:\n");
    for (int i = 0; i < MEMORY_ROWS; ++i) {
        for (int j = 0; j < MEMORY_COLS; ++j) {
            printf("%s  ", memory[i][j]);
        }
        printf("\n");
    }
}

int main() {
    for (int i = 0; i < MEMORY_ROWS; ++i) {
        for (int j = 0; j < MEMORY_COLS; ++j) {
            for(int k=0;k<BLOCK_SIZE;k++){
            memory[i][j][k] = 'A' + rand() % 26; 
            }
            memory[i][j][BLOCK_SIZE]='\0';
        
        }
    }
    struct CacheLine** cache=NULL;
    cache = initializeCache(&cache);
    printMemoryContents();
    printf("\n");
    printf("Reading from and writing to cache:\n");
    int address = rand() % (MEMORY_ROWS * MEMORY_COLS); 
    printf("address generated %d\n",address);
    printf("Reading from address %d: %c\n", address, readFromCache(address,cache));
    printf("Reading from address %d: %c\n", address, readFromCache(address,cache));
    char* data=malloc(sizeof(char)*5);
    strcpy(data,"HELO");
    address=47;
    printf("Writing to address %d: %s\n", address, data);
    writeToCache(address, data,cache);
    printf("\n");
    printf("Reading from address %d: %c\n", address, readFromCache(address,cache));
    printMemoryContents();
    return 1;
}