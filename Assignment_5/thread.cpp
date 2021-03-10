#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include<iostream>
#include<queue>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std; 

#define BufferSize 5 

sem_t empty;
sem_t full;

pthread_mutex_t mutex;



struct job
{
    pthread_t tid;
    int producer_num;
    int priority;
    int compute_time;
    int job_id;
};

struct compare 
{
    bool operator() (job const& j1, job const& j2)
    {
        return j1.priority < j2.priority;
    }
};

priority_queue<job,vector<job>,compare> PRQ;
int job_created = 0 ;
int job_completed = 0;

int NP,NC,jobs;

void *producer(void *pno)
{   
    int item;
    
   while(job_created <= jobs) 
   {

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        job j;
        j.priority = rand()%10 + 1;
        j.producer_num = *((int*)pno);
        j.compute_time = rand()%4 + 1;
        j.job_id = rand()%100000 + 1;
        j.tid = pthread_self();

        int sltime = rand()%4;
        sleep(sltime);
        
        PRQ.push(j);
        job_created ++ ;

        cout<<"producer:"<<j.producer_num<<" "<<"pro_thread id:"<<pthread_self()<<" "<<"job id: "<<j.job_id<<" priority:"<<j.priority<<" "<<"compute time:"<<j.compute_time<<" job created:"<<job_created<<"\n";

        pthread_mutex_unlock(&mutex);
        sem_post(&full);
    }
}
void *consumer(void *cno)
{   

    
    while(job_completed <= jobs) 
    {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        
        int sltime = rand()%4;
        sleep(sltime);
        int cons_no = *((int*)cno);
        job c = PRQ.top();
        PRQ.pop();
        job_completed ++;
        

        cout<<"consumer:"<<cons_no<<" "<<"con_thread id:"<<pthread_self()<<" "<<"job id: "<<c.job_id<<" priority:"<<c.priority<<" "<<"compute time:"<<c.compute_time<<" job completed:"<<job_completed<<"\n";
        
        sleep(c.compute_time);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

int main()
{   
    srand(time(0));
    cin>>NP>>NC>>jobs;
    pthread_t pro[NP+1],con[NC+1];
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty,0,BufferSize);
    sem_init(&full,0,0);

    int pros[NP+1],cons[NC+1];
    for(int i=0;i<NP;i++) {pros[i]=i;}
    for(int i=0;i<NC;i++) {cons[i]=i;}


    for(int i = 0; i < NP; i++) 
    {
        pthread_create(&pro[i], NULL, producer, (void *)&pros[i]);
    }
    for(int i = 0; i < NC; i++) 
    {
        pthread_create(&con[i], NULL, consumer, (void *)&cons[i]);
    }

    for(int i = 0; i < NP; i++) {
        pthread_join(pro[i], NULL);
    }

    for(int i = 0; i < NC; i++) {
        pthread_join(con[i], NULL);
    }
    
   
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
    
}
