#include<iostream>
#include<pthread.h>


class singleton
{
    public:
        static singleton* GetInstance();
    private:
        singleton();
        volatile static singleton* p;
        static pthread_mutex_t mutex;
};

singleton* singleton::GetInstance()
{
    if(p == NULL)
    {
        pthread_mutex_lock(&mutex);
        if(p == NULL)
        {
             p = new singleton();
        }
    }

    pthread_mutex_unlock(&mutex);
    return p;
}
