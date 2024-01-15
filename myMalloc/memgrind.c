#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

int main()
{
    int average1;
    int average2;
    int average3;
    int average4;
    int average5;
    int timeMaster; // Gives us the difference
    int counter = 0;
    struct timeval current_time;
    
    for (int i = 0; i < 50; i++) // Testing each task 50 times
    {    
    /*  
    ======================================================== Task 1 ========================================================
    */
        gettimeofday(&current_time, NULL);
        timeMaster = current_time.tv_usec;
    /*
    -------------------------------------------------------- START ---------------------------------------------------------
    */
        for(int i = 0; i < 120; i++) {
            char *ptr = malloc(1);  // Allocate 1 byte of memory
            free(ptr);  // Release the memory
        }
        printf("MemClear?: %d\n", memCleared());  // Check if memory is cleared

    /*
    -------------------------------------------------------- END -----------------------------------------------------------
    */
        gettimeofday(&current_time, NULL);
        average1 = average1 + (current_time.tv_usec - timeMaster);
        if (counter == 1) average1 = average1/2;
    /*  
    ======================================================== Task 2 ========================================================
    */
        gettimeofday(&current_time, NULL);
        timeMaster = current_time.tv_usec;
    /*
    -------------------------------------------------------- START ---------------------------------------------------------
    */
        
        char *ptr2Array[120];  // Array to store 120 pointers

        for(int i = 0; i < 120; i++) {
            ptr2Array[i] = malloc(1);  // Allocate 1 byte of memory and store the address
        }
        // //getSizes();

        for(int i = 0; i < 120; i++) {
            free(ptr2Array[i]);  // Release the memory
        }
        // //getSizes();

        printf("MemClear?: %d\n", memCleared());  // Check if memory is cleared 
    /*
    -------------------------------------------------------- END -----------------------------------------------------------
    */
        gettimeofday(&current_time, NULL);
        average2 = average2 + (current_time.tv_usec - timeMaster);
        if (counter == 1) average2 = average2/2;

/*  
    ======================================================== Task 3 ========================================================
    */
        gettimeofday(&current_time, NULL);
        timeMaster = current_time.tv_usec;

    /*
    -------------------------------------------------------- START ---------------------------------------------------------
    */
        char *ptr3Array[120];  // Array to store 120 pointers
        int allocated[120] = {0};  // Initialize the memory allocation status array
        int loc = 0;  // Current location

        for(int i = 0; i < 120; i++) {
            if(loc == 0 || (rand() % 2 == 0 && loc < 120)) {
                // Allocate 1 byte of memory and store the address
                printf("alloc loc=%d\n", loc);
                ptr3Array[loc] = malloc(1);
                allocated[loc] = 1;
                loc++;
            } else {
                // Release the memory
                loc--;
                printf("free loc=%d\n", loc);
                free(ptr3Array[loc]);
                allocated[loc] = 0;
            }
        }

        printf("Process is done.\n");

        // Clean up any unreleased memory
        for(int i = 0; i < 120; i++) {
            if(allocated[i] == 1) {
                free(ptr3Array[i]);
            }
        }

        printf("MemClear?: %d\n", memCleared());  // Check if memory is freed

    /*
    -------------------------------------------------------- END -----------------------------------------------------------
    */
        gettimeofday(&current_time, NULL);
        average3 = average3 + (current_time.tv_usec - timeMaster);
        if (counter == 1) average3 = average3/2;
    /*  
    ======================================================== Task 4 ========================================================
               
    */
        gettimeofday(&current_time, NULL);
        timeMaster = current_time.tv_usec;

/*
    -------------------------------------------------------- START ---------------------------------------------------------
*/
        int task4Test[120];
        int *task4TestM = malloc(120*sizeof(int));
        for (int i = 0; i < 120; i++ )
        {
            task4Test[i] = i;
            task4TestM[i] = i;
        }
        for (int i = 0; i < 120; i++)
        {
            if (task4Test[i] != task4TestM[i]) 
            {
                printf("Memory is bugged\n");
                break;
            }
        }
        free(task4TestM);
        printf("MemClear?: %d\n", memCleared());  // Check if memory is freed

    /*
    -------------------------------------------------------- END -----------------------------------------------------------
    */
        gettimeofday(&current_time, NULL);
        average4 = average4 + (current_time.tv_usec - timeMaster);
        if (counter == 1) average4 = average4/2;
    /*  
    ======================================================== Task 5 ========================================================
    This is going to test the boundries of the memory and see if they supply the same error results.
    */
        gettimeofday(&current_time, NULL);
        timeMaster = current_time.tv_usec;
    /*
    -------------------------------------------------------- START ---------------------------------------------------------
    */
        int *a = malloc(0);
        int *b = malloc (-1);
        int *c = malloc(4096);
        int *d = malloc(4088);
        free(a);
        free(b);
        free(c);
        free(d+1);
        free(d);
        free(d);

        printf("MemClear?: %d\n", memCleared());  // Check if memory is freed

    /*
    -------------------------------------------------------- END -----------------------------------------------------------
    */
        gettimeofday(&current_time, NULL);
        average5 = average5 + (current_time.tv_usec - timeMaster);
        if (counter == 1) average5 = average5/2;
    /*
    =================================================== END OF TEST TASKS ===================================================
    */

        counter = 1; // To use division and assist in taking the average next time around
    }
    printf("Average time of task 1: %d ms\n", average1);
    printf("Average time of task 2: %d ms\n", average2);
    printf("Average time of task 3: %d ms\n", average3);
    printf("Average time of task 4: %d ms\n", average4);
    printf("Average time of task 5: %d ms\n", average5);
            
}