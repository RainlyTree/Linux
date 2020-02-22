#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>

#define shm_key 0x12345688

int main()
{
    int shmid = shmget(shm_key, 1024, IPC_CREAT|0666);

    if(shmid < 0)
    {
        perror("shmget");
        return 0;
    }

    void* lp = shmat(shmid, NULL, 0);
    if(!lp)
    {
        perror("shmat");
        return 0;
    }

    while(1)
    {
        printf("read %s\n", (char*)lp);
    }

    shmdt(lp);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
