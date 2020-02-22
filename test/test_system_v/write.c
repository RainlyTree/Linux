#include<stdio.h>
#include<unistd.h>
#include<sys/shm.h>

#define  shm_key 0x12345688

int main()
{
    int shmid = shmget(shm_key, 1024, IPC_CREAT | IPC_EXCL |  0666);
    if(shmid < 0)
    {
        perror("shmget");
        return 0;
    }
    void *lp = shmat(shmid, NULL, 0);
    if(!lp)
    {
        perror("shmat");
        return 0;
    }

    while(1)
    {
        sprintf((char*)lp, "%s", "test-shm \n");
        sleep(1);
    }
    shmdt(lp);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
