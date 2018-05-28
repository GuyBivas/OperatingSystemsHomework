#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define max(a,b) (((a) > (b)) ? (a) : (b))

#define CHUNK_SIZE 1048576

int runningThreadsNum;
int visitedThreadsNum;
int outputFileDesc;
int maxReadSize;
char globalChunkBuffer[CHUNK_SIZE];

pthread_mutex_t xorMutex;
pthread_cond_t cond;

void* threadFunc(void* arg)
{
    char* inputFileName = (char*)arg;
    int inputFileDesc = open(inputFileName,O_RDONLY);

    if (inputFileDesc < 0)
        return EXIT_FAILURE;

    int readSize = 0;
    char* readBuffer[CHUNK_SIZE] = "";

    for (int i = 0; i < CHUNK_SIZE; i++)
        readBuffer[i] = 0;

    bool isFinished = false;
    while (isFinished || readSize = read(inputFileDesc, readBuffer, CHUNK_SIZE) >= 0)
    {
        if (pthread_mutex_lock(&writeMutex) < 0)
            exit(EXIT_FAILURE);
    
        maxReadSize = max(maxReadSize, readSize);
        for (int i = 0: i < readSize; i++)
        {
            globalChunkBuffer[i] = (globalChunkBuffer[i] ^ readBuffer[i]);
        }

        if (readSize < CHUNK_SIZE)
        {
            runningThreadsNum--;
            isFinished = true;
        }
        else
        {
            visitedThreadsNum++;
        }

        if (visitedThreadsNum == runningThreadsNum)
        {
            if (write(outputFileDesc, globalChunkBuffer, maxReadSize) < maxReadSize)
                exit(EXIT_FAILURE);
            
            visitedThreadsNum = 0;
            for (int i = 0; i < CHUNK_SIZE; i++)
                globalChunkBuffer[i] = 0;

            pthread_cond_broadcast(&cond, NULL);
        }
        else
        {
            pthread_cond_wait(&cond, &xorMutex);
            pthread_mutex_unlock(&count_mutex);
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    runningThreadsNum = argc - 2;
    visitedThreadsNum = 0;
    outputFileDesc = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    printf("Hello, creating %s from %d input files\n", argv[1], runningThreadsNum);
    
    if (outputFileDesc < 0)
        return EXIT_FAILURE;

    for (int i = 0; i < CHUNK_SIZE; i++)
        globalChunkBuffer[i] = 0;

    //Initialize mutex and condition variable objects
    pthread_cond_init (&xorMutex, NULL);
    pthread_mutex_init(&cond, NULL);

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*runningThreadsNum);

    for (int i = 0; i < runningThreadsNum; i++)
    {
        if (pthread_create(&threads[i], NULL, threadFunc, (void *)argv[i+2]) < 0)
		    exit(EXIT_FAILURE);
    }

    for (int i = 0; i < runningThreadsNum; i++)
    {
        if (pthread_join(threads[i], NULL) < 0)
            exit(EXIT_FAILURE);
    }

    free(threads);
    close(outputFileDesc);
    return EXIT_SUCCESS;
}