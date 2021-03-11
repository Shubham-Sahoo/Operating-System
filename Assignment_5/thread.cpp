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

#define BufferSize 8 

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

struct SHM
{
    priority_queue<job,vector<job>,compare> PRQ;
    int job_created  ;
    int job_completed ;
};


SHM memory;







int NP,NC,jobs;

void *producer(void *pno)
{   
    
    
   while( 1 ) 
   {

        
        
       // if(memory.job_created >= jobs) break;
        int sltime = rand()%4;
        sleep(sltime);

        if(memory.job_created < jobs)
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        
        if(memory.job_created < jobs)
        {

            job j;
            j.priority = rand()%10 + 1;
            j.producer_num = *((int*)pno);
            j.compute_time = rand()%4 + 1;
            j.job_id = rand()%100000 + 1;
            j.tid = pthread_self();

            memory.PRQ.push(j);
            memory.job_created ++ ;
            cout<<"producer:"<<j.producer_num<<" "<<"pro_thread id:"<<pthread_self()<<" "<<"job id: "<<j.job_id<<" priority:"<<j.priority<<" "<<"compute time:"<<j.compute_time<<" job created:"<<memory.job_created<<" queue size: "<<memory.PRQ.size()<<"\n";
        
                pthread_mutex_unlock(&mutex);
                sem_post(&full);

        }
        else
        {
            cout<<"producer "<<*((int*)pno)<<" thread terminated \n";
            pthread_mutex_unlock(&mutex);
            sem_post(&empty);
            
            break;
        }
        
        
        
        
        
    }
}
void *consumer(void *cno)
{   

    
    while(1) 
    {
           
        int sltime = rand()%4;
        sleep(sltime);
        int x;
        if(memory.job_completed < jobs) 
            sem_wait(&full);
        pthread_mutex_lock(&mutex);


        int cons_no = *((int*)cno);
        
        if(memory.job_completed < jobs)
        {
            job c = memory.PRQ.top();
            memory.PRQ.pop();
            memory.job_completed ++;
            x = c.compute_time;
            cout<<"consumer:"<<cons_no<<" "<<"con_thread id:"<<pthread_self()<<" "<<"job id: "<<c.job_id<<" priority:"<<c.priority<<" "<<"compute time:"<<c.compute_time<<" job completed:"<<memory.job_completed<<" queue size: "<<memory.PRQ.size()<<"\n";
        
             pthread_mutex_unlock(&mutex);
            sem_post(&empty);
            
            sleep(x);

        }
        else
        {
            cout<<"consumer "<< cons_no<<" thread terminated \n";
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            
            break;

        }
    }
}

int main()
{   
    srand(time(0));
    cin>>NP>>NC>>jobs;
    memory.job_created = 0;
    memory.job_completed = 0;
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
