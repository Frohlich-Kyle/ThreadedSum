//Author: Kyle Frohlich
//Name: Homework 2
//Purpose: Thread Functionality
//Date: 28Feb2024

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct _thread_data_t
{
    const int* data;
    int startInd;
    int endInd;
    long long int* totalSum;
    pthread_mutex_t* lock;
} thread_data_t;

int readFile(char*, int[]);
void* arraySum(void*);

int main(int argc, char* argv[])
{

    if(argc < 2 || argc != 4)
    {
        printf("Incorrect number of arguements given on the command line\n");
        printf("To run the executable, format is ./threaded_sum, number of threads, some.txt, lock use value (1 or 0)\n");
        printf("Terminating\n");
        return 0;
    }
    static int numArray[100000000];
    //argc = 4
    //argv exec, numThreads, file.txt, locking status
    int numNums = readFile(argv[2], numArray);
    int numThreads = atoi(argv[1]);
    int shouldLock = atoi(argv[3]);

    if(numThreads > numNums)
    {
        printf("Too many threads requested\n");
    }

    if(numNums == -1)
    {
        printf("Error reading file\n");
        return 1;
    }
    if(numThreads < 1)
    {
        printf("Please enter a number of threads that you'd like to calculate with number of threads, filename to open, and lock use value\n");
        return 0;   
    }

    //link in pdf says to use struct timeval and not clock_t
    struct timeval start, end;
    //struct timeval middle;

    pthread_mutex_t mutexLock;
    if(pthread_mutex_init(&mutexLock, NULL) != 0)
    {
        printf("Failed to initialize mutex\n");
    }
    //if its numNums is not divisible by numThreads, remainder is slapped onto the last one
    int remainder = numNums % numThreads;
    int pairedDown = 0, sizePortions = 0;
    int temp = 0;
    
    //homework pdf says to initalize this in main and grading rubric says to make as a global variable
    long long int totalSum = 0;

    if(remainder > 0)
    {
        //calculates how many portions the array needs to be cut into
        pairedDown = numNums - remainder;
        sizePortions = pairedDown / numThreads;
        // printf("%d\n", numNums);
        // printf("%d\n", pairedDown);
    }
    else
    {
        sizePortions = numNums / numThreads;
    }
    //start of time with threads
    gettimeofday(&start, NULL);

    thread_data_t threadArray[numThreads];

    for(int index = 0; index < numThreads; index++)
    {
        //gets the start point
        temp = index * sizePortions;
        //assign pointer to array so it can access the goods
        threadArray[index].data = numArray;
        //assigns start point
        threadArray[index].startInd = temp;
        //assigns end point based on start point with da -1 for +1 error
        threadArray[index].endInd = threadArray[index].startInd + sizePortions - 1;
        //assigns total sum to each thread
        threadArray[index].totalSum = &totalSum;
        //assign lock
        if(shouldLock == 1)
        {
            threadArray[index].lock = &mutexLock;
        }
        else
        {
            threadArray[index].lock = NULL;
        }
    }
    if(remainder > 0)
    {
        //if there is a remainder, slaps it into the last thread
        threadArray[numThreads - 1].endInd += remainder;
    }

    //im assuming this one was a typo in the instructions as it asks for 3 different times
    //gettimeofday(&middle, NULL);


    pthread_t pthreadArray[numThreads];

    for(int index = 0; index < numThreads; index++)
    {
        pthread_create(&pthreadArray[index], NULL, arraySum, (void *)&threadArray[index]);
    }
    for(int index = 0; index < numThreads; index++)
    {
        pthread_join(pthreadArray[index], NULL);
    }

    gettimeofday(&end, NULL);

    //using the method outlined in the pdf image and link
    unsigned long long end_time = (end.tv_sec * 1000000) + (end.tv_usec);
    unsigned long long start_time = (start.tv_sec * 1000000) + (start.tv_usec);

    // the correct way to convert to milliseconds I believe
    // unsigned long long end_time = (end.tv_sec * 1000) + (end.tv_usec / 1000);
    // unsigned long long start_time = (start.tv_sec * 1000) + (start.tv_usec / 1000);
    printf("Sum of all Numbers: %lld\n", totalSum);
    printf("Time taken: %lld milliseconds\n", end_time - start_time);

    if(shouldLock == 1)
    {
        pthread_mutex_destroy(&mutexLock);
    }

    return 0;
}


int readFile(char* argv, int numArray[])
{
    FILE* inputFile = fopen(argv, "r");
    
    if(inputFile == NULL)
    {
        //printf("Error opening file");
        return -1;
    }
    else
    {
        int numNums = 0, readNum;

        while(fscanf(inputFile, "%d", &readNum) == 1)
        {
            numArray[numNums] = readNum;
            numNums++;
        }
        fclose(inputFile);

        if(numNums == 0)
        {
            return 1;
        }

        return numNums;
    }
}


void* arraySum(void* threadSlice)
{
    long long int threadSum = 0;
    thread_data_t* slicedThread = (thread_data_t*)threadSlice;

    for(int index = slicedThread->startInd; index < slicedThread->endInd + 1; index++)
    {
        threadSum += slicedThread->data[index];
    }

    if(slicedThread->lock != NULL)
    {
        pthread_mutex_lock(slicedThread->lock);
    }

    *(slicedThread->totalSum) += threadSum;

    if(slicedThread->lock != NULL)
    {
        pthread_mutex_unlock(slicedThread->lock);
    }

    return slicedThread;
}
