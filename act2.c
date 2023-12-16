/*
act2.c

Create mutex 
Create conditional variables

Create global shared memory with counters and matrix

create 2 threads
wait for both threads to finish
print info

thread1: LOOP till 100 numbers generated, print info
thread2: LOOP till 100 numbers generated, print info

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

#define SIZE 100
#define LOOP 10
#define FIRST 1 // first thread to be executed
#define SECOND 2
#define LRAND 0
#define BRAND 99
#define NONEW 0
#define NEW 1


// mutex to be used to synchronize both threads
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// conditional variables to manage threads alternance
pthread_cond_t cond_var1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_var2 = PTHREAD_COND_INITIALIZER;

int current_thread = FIRST; // this will be the first thread to be executed

void err_sys(const char* text)
{
    perror(text);
    exit(1);
}

struct shmseg {
	int total;
	int counts;
	int countc;
	char memory[SIZE];
};

struct shmseg shm;

// function to handle thread 1 (first player)
void *handle_thread1(void *vargp)
{
   int Zs; //random number generated
   int i, j;

   srand(time(NULL)+getpid());

   while (1)
   {
	// check mutex is ready
	pthread_mutex_lock(&mutex);
	// critical zone
	if (current_thread == SECOND)
	{
	// unlock mutex and block thread1
	  pthread_cond_wait(&cond_var1, &mutex);
	}

	//time for thread1
	// next thread after mutex unlock will be thread2
	current_thread = SECOND;

       //just in the critical zone
       //generate random number
        Zs = (rand() % (BRAND - LRAND +1)) + LRAND;
        if (shm.total >= SIZE) break;
        if (shm.memory[Zs] == '0')
        {
             shm.memory[Zs] = '1';
             shm.counts += 1;
             printf("T1: handler 1: %d-%d\n", shm.counts, shm.total);
             shm.total += 1;
             if (shm.total >= SIZE) break;
        }
        //exiting critical zone
	//end crtical zone
        //awake thread2
        pthread_cond_signal(&cond_var2);
	//release mutex
	pthread_mutex_unlock(&mutex);
   }
// To guarantee thread2 will have the option to have a last iteration
    pthread_cond_signal(&cond_var2);
    pthread_mutex_unlock(&mutex);
    printf("T1: End of thread 1\n");
    printf("T1: The total counter is %d\n", shm.total);
    printf("T1: The handler1 counter is %d\n", shm.counts);
    printf("T1: The handler2 counter is %d\n", shm.countc);
    printf("T1: the values for matrix are:\n");
    for (i = 0; i < LOOP; i ++)
    {
	printf("T1: ");
        for (j=0; j < LOOP; j++)
        {
            printf("%c ", shm.memory[i*LOOP+j]);
        }
        printf("\n");
    }
}

// function to handle thread 2 (second player)
void *handle_thread2(void *vargp)
{
   int Zs; //random number generated
   int i, j;
   int new; //new value on matrix

   srand(time(NULL)+getpid());

   while (1)
   {
        // check mutex is ready
        pthread_mutex_lock(&mutex);
        // critical zone
        if (current_thread == FIRST)
        {
        // unlock mutex and block thread1
          pthread_cond_wait(&cond_var2, &mutex);
        }

        //time for thread1
        // next thread after mutex unlock will be thread2
        current_thread = FIRST;
	new = NONEW;

	while (new == NONEW)
	{
       //just in the critical zone
       //generate random number
          Zs = (rand() % (BRAND - LRAND +1)) + LRAND;
          if (shm.total >= SIZE) break;
          if (shm.memory[Zs] == '0')
          {
             shm.memory[Zs] = '2';
             shm.countc += 1;
             printf("T2: handler 2: %d-%d\n", shm.countc, shm.total);
             shm.total += 1;
	    new = NEW;
          }
	}
        if (shm.total >= SIZE) break;

        //exiting critical zone
        //end crtical zone
	//awake thread1
	pthread_cond_signal(&cond_var1);
        //release mutex
        pthread_mutex_unlock(&mutex);
   }
// To guarantee thread2 will have the option to have a last iteration
    pthread_cond_signal(&cond_var1);
    pthread_mutex_unlock(&mutex);
    printf("T2: End of thread 2\n");
    printf("T2: The total counter is %d\n", shm.total);
    printf("T2: The hanlder1 counter is %d\n", shm.counts);
    printf("T2: The handler2 counter is %d\n", shm.countc);
    printf("T2: the values for matrix are:\n");
    for (i = 0; i < LOOP; i ++)
    {
	printf("T2: ");
        for (j=0; j < LOOP; j++)
        {
            printf("%c ", shm.memory[i*LOOP+j]);
        }
        printf("\n");
    }

}

int main(int argc, char *argv[])
{
//to manage the two threads needed on the program
    pthread_t handleThread_id1;
    pthread_t handleThread_id2;

    int     sem_value;
    int     result;
    int shmid;
    struct shmseg *shmp;
    int i, j;

//set values for counters and memory segment
    shm.total=0;
    shm.counts=0;
    shm.countc = 0;
    memset((char*)(shm.memory),'0',SIZE);

//print values to show right initialization
    printf("Just after initialization\n");
    printf("The total counter is %d\n", shm.total);
    printf("The handler1 counter is %d\n", shm.counts);
    printf("The handler2 counter is %d\n", shm.countc);
    printf("the values for matrix are:\n");
    for (i = 0; i < LOOP; i ++)
    {
        for (j=0; j < LOOP; j++)
        {
            printf("%c ", shm.memory[i*LOOP+j]);
        }
        printf("\n");
    }

// create noth threads
    pthread_create(&handleThread_id1, NULL, handle_thread1, NULL);
    pthread_create(&handleThread_id2, NULL, handle_thread2, NULL);

// wait end of both threads 
    result = pthread_join(handleThread_id1, NULL);
    if (result > 0) err_sys("Error joining thread 1 at end of program");
    result = pthread_join(handleThread_id2, NULL);
    if (result > 0) err_sys("Error joining thread 2 at end of program");

    printf("End of Game\n");
    printf("The total counter is %d\n", shm.total);
    printf("The handler 1 counter is %d\n", shm.counts);
    printf("The handler 2 counter is %d\n", shm.countc);
    printf("the values for matrix are:\n");
    for (i = 0; i < LOOP; i ++)
    {
        for (j=0; j < LOOP; j++)
        {
            printf("%c ", shm.memory[i*LOOP+j]);
        }
        printf("\n");
    }

}
