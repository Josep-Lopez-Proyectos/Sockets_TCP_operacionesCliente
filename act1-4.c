/*
act1-4.c
Second player
Link to semaphores
Link to shm segment

LOOP
  wait(sem2)
    do
  signal(sem1)
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
#define LRAND 0
#define BRAND 99

#define NAME1 "/semaphore1"
#define NAME2 "/semaphore2"

#define LOOP 10

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

int main(int argc, char *argv[])
{
    sem_t*  psem1;  //semaphore that control act1-3
    sem_t*  psem2; //semaphore that control act1-4
    int     sem_value;
    int     result;
    int shmid;
    int loop = 0;
    int Zs;
    struct shmseg *shmp;
    int i, j;

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

   while (1)
   {
	if (sem_wait(psem2)<0) err_sys("WAIT psem2");
	//just in the critical zone
       //generate random number
  	Zs = (rand() % (BRAND - LRAND +1)) + LRAND;
	if (shmp->total >= SIZE) break;
        if (shmp->memory[Zs] == '0')
        {
	     shmp->memory[Zs] = '2';
             shmp->countc += 1;
	     shmp->total += 1;
	     printf("act1-4: %d-%d\n", shmp->countc, shmp->total);
	     if (shmp->total >= SIZE) break;
        }
	//exiting critical zone
	if (sem_post(psem1)<0) err_sys("SIGNAL psem1");
   }

   if (sem_post(psem1)<0) err_sys("SIGNAL psem1");

    printf("The total counter is %d\n", shmp->total);
    printf("The act1-3 counter is %d\n", shmp->counts);
    printf("The act1-4 counter is %d\n", shmp->countc);
    printf("the values for matrix are:\n");
    for (i = 0; i < LOOP; i ++)
    {
        for (j=0; j < LOOP; j++)
        {
            printf("%c ", shmp->memory[i*LOOP+j]);          
        }
        printf("\n");
    }


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
