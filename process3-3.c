/*
First player server
Link to semaphores
Link to shm segment

LOOP
  wait(sem1)
    do
  signal(sem2)
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>

#define SHM_KEY 0x1234
#define SIZE 100

#define VSMALLER 0
#define VGREATER 2
#define VEQUAL 1

#define SMALLER "smaller than"
#define GREATER "greater than"
#define EQUAL "equal to"


#define LRAND 0
#define BRAND 100

#define NAME1 "/semaphore1"
#define NAME2 "/semaphore2"

void err_sys(const char* text)
{
    perror(text);
    exit(1);
}

struct shmseg {
	int client;
	int iteration;
	int comparison;
};

int compare(int n1,int n2);


int main(int argc, char *argv[])
{
    sem_t*  psem1;  //semaphore that control act3-3
    sem_t*  psem2; //semaphore that control act3-4
    int     sem_value;
    int     result;
    int shmid;
    int Zs;
    int Zc;
    int iteration;
    struct shmseg *shmp;
    int i, j;
    char message[3][20];

    strcpy(message[0], SMALLER);
    strcpy(message[1], EQUAL);
    strcpy(message[2], GREATER);


    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666);
    if (shmid == -1) err_sys("Shared Memory Error");

    //attach to the shmp pointer
    shmp = shmat(shmid, NULL, 0);
    if (shmp == (void*)(-1)) err_sys("Shared Memory attachment error");

    /* Link psem1 */
    psem1 = (sem_t*)sem_open(NAME1, 0, 0600, 0);
    if (psem1 == SEM_FAILED) {
        err_sys("Open psem1");
    }
    /* Read and print psem1 */
    result = sem_getvalue(psem1, &sem_value);
    if (result < 0) {
        err_sys("Read psem1");
    }
    printf("SEM1: %d\n", sem_value);

    /* Link psem2 */
    psem2 = (sem_t*)sem_open(NAME2, 0, 0600, 0);
    if (psem2 == SEM_FAILED) {
        err_sys("Open psem2");
    }

    /* Read and print psem2 */
    result = sem_getvalue(psem2, &sem_value);
    if (result < 0) {
        err_sys("Read psem2");
    }
    printf("SEM2: %d\n", sem_value);

    srand(time(NULL)+getpid());
    //generate random number
    Zs = (rand() % (BRAND - LRAND +1)) + LRAND;
    iteration = shmp->iteration;
    if (sem_wait(psem1)<0) err_sys("WAIT psem1");
    if (sem_post(psem2)<0) err_sys("SIGNAL psem2");

//next to be executed client

   while (1)
   {
	if (sem_wait(psem1)<0) err_sys("WAIT psem1");
	//just in the critical zone
	// client has generated random number
       Zc = shmp->client;
       // compare values
      result = compare(Zc,Zs);
      shmp->comparison = result;
      iteration += 1;
      shmp->iteration = iteration;
      printf("Iteration %d, client number %d, result is %s\n", shmp->iteration, shmp->client, message[result]);
      if (sem_post(psem2)<0) err_sys("SIGNAL psem2");
      if ( result == VEQUAL) break;
   }

    printf("The server number is %d\n", Zs);
    printf("The number of iterations has been %d\n", shmp->iteration);

    /* Close psem1 */
    result = sem_close(psem1);
    if (result != 0) {
        err_sys("Close psem1");
    }

    /* Close psem2 */
    result = sem_close(psem2);
    if (result != 0) {
        err_sys("Close psem2");
    }

    //dettach the shmp pointer
    result = shmdt(shmp);
    if (result == (-1)) err_sys("Shared Memory dettachment error");

}



int compare(int n1, int n2)
{
        if (n1 < n2) return(0);
        if (n1 == n2) return(1);
        if (n1 > n2) return(2);
}

