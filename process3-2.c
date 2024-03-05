/*
delete semaphores
delete shm segment
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
#include <sys/types.h>
#include <string.h>

#define SHM_KEY 0x1234
#define SIZE 100
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

int main(int argc, char *argv[])
{
    sem_t*  psem1;  //semaphore that control act1-3
    sem_t*  psem2; //semaphore that control act1-4
    int     sem_value;
    int     result;
    int shmid;
    struct shmseg *shmp;

    shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0666|IPC_CREAT);
    if (shmid == -1) err_sys("Shared Memory Error");
    result = shmctl(shmid, IPC_RMID, NULL);
    if (result == (-1)) err_sys("Shared Memory remove  error");
    printf("shm segment removed\n");

    if (sem_unlink(NAME1)!=0) err_sys("UNLINK 1");
    if (sem_unlink(NAME2)!=0) err_sys("UNLINK 2");
    printf("semaphores unlinked\n");
}
