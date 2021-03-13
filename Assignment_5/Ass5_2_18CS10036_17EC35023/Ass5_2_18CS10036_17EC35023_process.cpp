#include <iostream> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <sys/types.h>
#include<sys/wait.h>  
#include <unistd.h> 
#include <string>
#include <queue>
#include <pthread.h>
#include <mutex> 
#include<time.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/mman.h>
using namespace std;

#define SEM_NAME "/semaphore_example5"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

static int prod_number;
int total_jobs;

struct job
{
	int proc_id;
	int prod_no;
	int priority;
	int cp_time;
	long int job_id;

	//job(pid_t proc_id, int prod_no, int priority, int cp_time, long int job_id)
    //    : proc_id(proc_id), prod_no(prod_no),priority(priority),cp_time(cp_time),job_id(job_id)
    //{
    //}
};

struct Compare_pr {
    bool operator()(job const& p1, job const& p2)
    {
        // return "true" if "p1" is ordered 
        // before "p2", for example:
        return p1.priority < p2.priority;
    }
};


struct shm
{
	//priority_queue<job, vector<job>, Compare_pr> prq;
	job pq[8];
	int size;
	int job_created;
	int job_completed;
};


int search_pos(int pr, job q[], int sz)
{
	for(int i=0;i<sz;i++)
	{
		if(pr>=q[i].priority)
		{
			return i;
		}
	}
	return sz;
}

void producer(int *id)
{	
	srand(*id);

	int r = rand();
	r = r%4;
	//cout<<"Process "<<getpid()<<" "<<r<<endl;
	sleep(r);

	//key_t key4 = 0x1001;
	
	sem_t *semaphore = sem_open(SEM_NAME, O_EXCL, SEM_PERMS, INITIAL_VALUE);


    while (semaphore == SEM_FAILED) {
        //perror("sem_open(3) error");
        //exit(EXIT_FAILURE);
        //producer(id);
        //return;
    	semaphore = sem_open(SEM_NAME, O_EXCL, SEM_PERMS, INITIAL_VALUE);


    }
	
	//int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	//pthread_mutex_t *lock;

	//lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	// if (pthread_mutex_init(lock, NULL) != 0) { 
 //        printf("\n mutex init has failed\n"); 
 //        return; 
 //    }
	
    //pthread_mutex_lock(lock);

	key_t key1 = 0x1002;
	// key_t key2 = 0x1028;
	// key_t key3 = 0x1025;

	int id1 = shmget(key1, 1024, IPC_EXCL | 0666);
	// int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	//int *job_created,*job_completed;
	

	// job_created = (int*)shmat(id1, NULL, 0);
	// job_completed = (int*)shmat(id2, NULL, 0);

	// priority_queue<job, vector<job>, Compare_pr> *cqueue;
	// cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);

	shm *memory;

	memory = (shm*)shmat(id1, NULL, 0);
	//cout<<"hi"<<flush;
	//cout<<memory->job_created<<" "<<memory->job_completed<<" "<<total_jobs<<" "<<memory->size<<endl<<flush;
	//srand(time(NULL));
	int pr = 1+(rand()%10);
	//srand(time(NULL));
	long int j_id = 1+(rand()%100000);
	//srand(time(NULL));
	int cmp = 1+(rand()%4);

	job el;
	el.proc_id = getpid();
	el.prod_no = *id;
	el.priority = pr;
	el.cp_time = cmp;
	el.job_id = j_id;
	//job el(0,1,3,2,);
	if(memory->job_created<total_jobs)
	{	
		if(memory->size<8)
		{	
			
			memory->job_created += 1;
			// cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl;
			cout<<"Producer : "<<el.prod_no<<" ";
			cout<<"Producer pid : "<<el.proc_id<<" ";
			cout<<"Priority : "<<el.priority<<" ";
			cout<<"Compute Time : "<<(el.cp_time)<<endl<<endl<<flush;
			//(memory->prq).push(el);

			//cout<<"Check val : "<<(memory->pq[0]).priority<<endl;
			
			
			
				//memory->pq[*id] = el;
			int p = search_pos(pr,memory->pq,memory->size);
			job temp;
			for(int i=memory->size+1;i>p;i--)
			{
				memory->pq[i] = memory->pq[i-1]; 
			}
			memory->pq[p] = el;
			memory->size +=1;
			
			
		}
		
		//cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		
	}

	
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	
	//shmdt(job_completed);
	//shmdt(cqueue);
	int jc,jcr;
	jc = memory->job_created;
	jcr = memory->job_completed;

	shmdt(memory);

	//pthread_mutex_unlock(lock);

	//pthread_mutex_destroy(lock);
	//shmdt(lock);

    if (sem_close(semaphore) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }
	//sleep(1);
	if(jc<total_jobs)
	{	
		//cout<<"Here"<<endl<<flush;
		//shmdt(job_created);
		producer(id);
		
	}
	return;
}

void consumer(int *id)
{	
	//srand(getpid());
	srand(*id);
	int r = rand()%4;
	sleep(r);

	//key_t key4 = 0x1001;
	//sem_close(semaphore);
	//sem_unlink(SEM_NAME);
	sem_t *semaphore = sem_open(SEM_NAME, O_EXCL, SEM_PERMS, INITIAL_VALUE);


    while(semaphore == SEM_FAILED) {
        //perror("sem_open(3) error");
        //exit(EXIT_FAILURE);
        //consumer(id);
        //return;
        semaphore = sem_open(SEM_NAME, O_EXCL, SEM_PERMS, INITIAL_VALUE);
    }

	//int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	//pthread_mutex_t *lock;
	//lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	// if (pthread_mutex_init(lock, NULL) != 0) { 
 //        printf("\n mutex init has failed\n"); 
 //        return; 
 //    }
    //pthread_mutex_lock(lock);
	
	key_t key1 = 0x1002;
	// key_t key2 = 0x1028;
	// key_t key3 = 0x1025;

	int id1 = shmget(key1, 1024, IPC_EXCL | 0666);
	// int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	// int *job_created,*job_completed;
	

	// job_created = (int*)shmat(id1, NULL, 0);
	// job_completed = (int*)shmat(id2, NULL, 0);

	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);

	shm *memory;
	memory = (shm*)shmat(id1, NULL, 0);

	//priority_queue<job, vector<job>, Compare_pr> qp_val;
	//qp_val = *queue;

	//cout<<"Consumer "<<memory->size<<" "<<(memory->pq[0]).priority<<endl;
	
	int sl_time=0;
	if(memory->job_completed<memory->job_created)
	{
		if(memory->size>0)
		{	
			//job el(0,0,0,0,0);
			job el;
			
			el = memory->pq[0];
			for(int i=0;i<memory->size;i++)
			{
				memory->pq[i] = memory->pq[i+1]; 
			}
			memory->size -= 1;
			

			//job el = memory->pq[*id];//(memory->prq).top();
			//el.cp_time = 4;
			//cout<<"check : "<<((*queue).top()).cp_time<<endl;
			memory->job_completed += 1;
			

			cout<<"Consumer : "<<*id<<" ";
			cout<<"Consumer pid : "<<getpid()<<" ";
			cout<<"Producer : "<<el.prod_no<<" ";
			cout<<"Producer pid: "<<el.proc_id<<" ";
			cout<<"Priority : "<<el.priority<<" ";
			cout<<"Compute Time : "<<(el.cp_time)<<endl<<endl<<flush;
			sl_time = el.cp_time;
			//(memory->prq).pop();


				
			
		}
		else
		{
			// shmdt(job_created);
			// shmdt(job_completed);
			// shmdt(cqueue);
			// pthread_mutex_unlock(lock);

			// pthread_mutex_destroy(lock);
			
			// shmdt(lock);
			// el = cqueue->top();
			// cqueue->pop();
			//*job_completed -= 1;
		}
		// cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		
		

	}

	
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	
	// shmdt(queue);
	int jc,jcr;
	jc = memory->job_created;
	jcr = memory->job_completed;

	shmdt(memory);
	//pthread_mutex_unlock(lock);

	// pthread_mutex_destroy(lock);
	
	//shmdt(lock);

    if (sem_close(semaphore) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }
	//cout<<"hi"<<flush;
	sleep(sl_time);
	if(jcr<jc)
	{	
		//cout<<"hi"<<endl<<flush;
		//shmdt(job_completed);
		//shmdt(job_created);
		consumer(id);
		//exit(0);
	}
	
	return;
}

int main()
{	

	int no_p,no_c,no_j;
	cout<<"Enter number of producers :"<<endl;
	cin>>no_p;
	cout<<"Enter number of consumers :"<<endl;
	cin>>no_c;
	cout<<"Enter the number of jobs :"<<endl;
	cin>>(::total_jobs);
	//cout<<total_jobs; 

	sem_unlink(SEM_NAME);
	sem_t *semaphore = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);


    if (semaphore == SEM_FAILED) {
        perror("sem_open(3) error");
        exit(EXIT_FAILURE);
    }

	//job p_queue[8];
	// priority_queue<job, vector<job>, Compare_pr> p_queue;
	// cout<<sizeof(p_queue);
	// job a(1,1,5,4,2);
	// a.proc_id = 1;
	// a.prod_no = 1;
	// a.priority = 5;
	// a.cp_time = 0;
	// a.job_id = 2;
	// p_queue.push(a);
	// job s = p_queue.top();
	// cout<<(s).cp_time<<endl;	
	key_t key1 = 0x1002;
	key_t key4 = 0x1001;
	int shmid1 = shmget(key1, 1024, IPC_CREAT | 0666);

	//int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

	//int *job_created,*job_completed;
	//cout<<&myseg<<endl;
	// job_created = (int*)shmat(shmid1, NULL, 0);
	// job_completed = (int*)shmat(shmid2, NULL, 0);

	shm *memory;
	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(shmid3, NULL, 0);

	memory = (shm*)shmat(shmid1, NULL, 0);

	//cout<<myseg<<flush;
	//cout<<*myseg<<endl<<flush;
	//int a = 23;	
	//pthread_mutex_t *lock;
	//pthread_mutexattr_t mutex_attr;
	//pthread_mutexattr_init(&mutex_attr);
	//pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
	//pthread_mutex_init(&(memory->mutex), &mutex_attr);

	// lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
	// if (pthread_mutex_init(lock, NULL) != 0) { 
 //        printf("\n mutex init has failed\n"); 
 //        return 1; 
 //    } 

    //pthread_mutex_lock(lock); 

	memory->job_created = 0;
	memory->job_completed = 0; 
	memory->size = 0;
	//cout<<*job_created<<" "<<*job_completed<<" "<<endl<<flush;

	
	//queue->push(a);

	int val1 = shmdt(memory);
	// int val2 = shmdt(job_completed);
	// int val3 = shmdt(queue);
	//int val4 = shmdt(lock);
	//cout<<val1<<" "<<"\n"<<endl<<flush;

	//pthread_mutex_unlock(lock);

	if (sem_close(semaphore) < 0) {
        perror("sem_close(3) failed");
        /* We ignore possible sem_unlink(3) errors here */
        sem_unlink(SEM_NAME);
        exit(EXIT_FAILURE);
    }

	// pthread_mutex_destroy(lock);
	// key_t key = ftok("shmfile",65); 
  
 //    // shmget returns an identifier in shmid 
 //    int shmid = shmget(key,1024,0666|IPC_CREAT); 
  
 //    // shmat to attach to shared memory 
 //    char *str = (char*) shmat(shmid,(void*)0,0); 
  
 //    cout<<"Write Data : "; 
 //    string s;
 //    getline(cin, s); 
 //  	str = &s[0];
 //    printf("Data written in memory: %s\n",str); 
      
 //    //detach from shared memory  
 //    shmdt(str); 
    //char *st = (char *)shmat(shmid,(void*)0,0); 
	  
	//printf("Data read from memory: %s\n",st);

	time_t start = time(NULL);
	prod_number = 0;
	for(int i=0;i<no_p;i++)
	{
		if(fork()==0)
		{	
			producer(&i);

			exit(0);
		}
	}
	prod_number = 0;
	// for(int i=0;i<no_p;i++)
	// {
	// 	wait(NULL);
	// }
	for(int i=0;i<no_c;i++)
	{
		if(fork()==0)
		{	
			consumer(&i);
			//cout<<"cons process"<<endl;
			exit(0);
		}
	}

	for(int i=0;i<no_p+no_c;i++)
	{
		wait(NULL);
	}


    	cout<<"total execution time of jobs "<<(double)(time(NULL) - start) <<" seconds \n";

	//pthread_mutex_destroy(lock);
	shmctl(shmid1, IPC_RMID, NULL);
	sem_unlink(SEM_NAME);
	// shmctl(shmid2, IPC_RMID, NULL);
	// shmctl(shmid3, IPC_RMID, NULL);
	//shmctl(shmid4, IPC_RMID, NULL);
	return 0;
	
}
