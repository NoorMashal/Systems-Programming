#include <stdio.h>

#define MEMSIZE 4096


#define ROUNDUP8(x) (((x) + 7) & (-8))

char memory[MEMSIZE];

struct block{
    int flag;   // indicates whether the block is free
    int size;    // indicates the size of the block
};

struct block *startblock = (void*)memory;

int memCleared() // checks if the memory is free
{
    int memResult = 0;
    struct block *memCurrent = startblock;
    while(((char *)memCurrent - memory) < MEMSIZE)
    {
        // printf("flagMemClear: %d\n", memCurrent->flag);
        // printf("Address: %ld\n", memCurrent);
        if (memCurrent->flag == 1)// not free
        {
            
            return ++memResult;
        }
        memCurrent = memCurrent + 1;
    }
    return memResult;
}

void connector() // Connects 2 chunks that are free in a row
{
    struct block *walker, *prev;
    walker = startblock;
    int counter = 0;
    while( walker < (struct block*)(memory + MEMSIZE) )
    {
        if (counter == 1 && walker->flag == 0)
        {
            prev->size = walker->size + prev->size +sizeof(struct block);
            //printf("new size: %d\n", prev->size);
            walker->size = 0;
            return;
        }
        else if (walker->flag == 0) 
        {
            counter++;
            prev = walker;
        }
        else counter = 0;
        //printf("%d\n",walker->size);

        walker = walker + ((walker->size + sizeof(struct block))/8);
    }
}

void myfree(void* addy, const char* file, int line){
    if( ( (void*)memory <= addy ) && ( addy <= (void*)(memory+MEMSIZE) ) )
    {
        struct block* curr;
        curr = addy;
        curr = curr - 1;
        //printf("This is the current FreeSize: %d\n", curr->size);
        
        if (curr->size == NULL)
        {
            printf("ERROR: Pointer not at the start of the chunk\n"); //error
            return;
        }

        if(curr->flag == 0) 
        {
            printf("ERROR: Cannot double free\n"); //ERROR
            return;
        }

        curr->flag=0;
        //printf("Freeing memory succesful.\n");
    }
    else
    {
        printf("ERROR: This pointer is not valid.\n");
        return;
    }
    connector();
    connector();
}


void *mymalloc(int bytes, const char* file, int line)
{
    bytes = ROUNDUP8(bytes);
    if (bytes <= 0) // ERROR
    {
        printf("ERROR: Invalid memory allocation request. Requested size must be a positive number.\n");
        return;
    }
    if (bytes > MEMSIZE - sizeof(struct block)) // ERROR
    {
        printf("ERROR: Memory allocation failed. Requested size %d bytes exceeds available memory %d bytes.\n", bytes, MEMSIZE-sizeof(struct block));
        return;
    }
    //printf("bytes: %d\n", bytes);
    struct block *current;
    void *result;

    if(!(startblock->size)) // initializes the memroy
    {
        startblock->size=MEMSIZE-sizeof(struct block);
        startblock->flag=0;
    }

    current = memory;

    while(((char *)current - memory) < MEMSIZE)
    {
        // printf("current flag: %d\n", current->flag);
        if ( current->flag == 0 )
        {
            if(current->size==(bytes)) // the final chunk is exactly the same size
            {
                current->size=bytes;
                current->flag=1;
                result=(void*)(++current);
                // printf("block allocated. 1 \n");

                return result;
            }

            if(current->size > bytes && current->size < (sizeof(struct block) + bytes)) // the final chunk is more than the size but not enough to make a new one
            {
                current->flag=1;
                result=(void*)(++current);
                // printf("block allocated. 2 \n");

                return result;
            }

            if(current->size >= (bytes + sizeof(struct block))) // the final chunk has enough space for the current chunk and to make a new block for the next chunk
            {
                // create new next block
                struct block *newblock = current + ((bytes + sizeof(struct block))/8);
                //printf("newblock address: %ld\n", newblock);
                newblock->size = (current->size) - bytes - sizeof(struct block);
                newblock->flag = 0;
                //printf("new Size of next block: %d\n", newblock->size);

                current->size=bytes;
                current->flag=1;
                result=(void*)(++current);
                //printf("block allocated. 3 \n");

                return result;
            }
        }
        current = current + ((current->size + sizeof(struct block))/8);
    }

    printf("Error: no enough memory\n");
    return NULL;

}
