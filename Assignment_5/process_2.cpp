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
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/mman.h>
using namespace std;


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

void producer(int *id)
{	
	srand(getpid());

	int r = rand();
	r = r%4;
	cout<<"Process "<<getpid()<<" "<<r<<endl;
	sleep(r);

	// key_t key4 = 0x1031;
	const char* key4 = "test5";
	
	
	//int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	pthread_mutex_t *lock;
	int shmid4 = shm_open(key4, O_RDWR, 0666);
	//lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
	lock = (pthread_mutex_t*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid4, 0);
	// if (pthread_mutex_init(lock, NULL) != 0) { 
 //        printf("\n mutex init has failed\n"); 
 //        return; 
 //    }
    pthread_mutex_lock(lock);

	// key_t key1 = 0x1027;
	// key_t key2 = 0x1028;
	// key_t key3 = 0x1025;

	const char* key1 = "test2";
	const char* key2 = "test3";
	const char* key3 = "test4";

	// int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	// int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// int id3 = shmget(key3, 512, IPC_EXCL | 0666);


	int shmid1 = shm_open(key1, O_RDWR, 0666);
	int shmid2 = shm_open(key2, O_RDWR, 0666);
	int shmid3 = shm_open(key3, O_RDWR, 0666);
	
 
	
	int *job_created,*job_completed;
	

	// job_created = (int*)shmat(id1, NULL, 0);
	// job_completed = (int*)shmat(id2, NULL, 0);

	job_created = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid1, 0);
	job_completed = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid2, 0);
	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *queue;
	queue = (priority_queue<job, vector<job>, Compare_pr>*)mmap(0, 1024, PROT_WRITE, MAP_SHARED, shmid3, 0);

	// priority_queue<job, vector<job>, Compare_pr> *cqueue;
	// cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);

	//cout<<"hi"<<flush;
	cout<<*job_created<<" "<<*job_completed<<" "<<total_jobs<<" "<<queue->size()<<endl<<flush;
	int pr = 1+(rand()%10);
	long int j_id = 1+(rand()%100000);
	int cmp = 1+(rand()%4);

	job el;
	el.proc_id = getpid();
	el.prod_no =*id;
	el.priority = pr;
	el.cp_time = cmp;
	el.job_id = j_id;
	//job el(0,1,3,2,);
	if(*job_created<total_jobs)
	{	
		if(queue->size()<8)
		{	
			
			*job_created += 1;
			// cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl;
			cout<<"Producer : "<<el.prod_no<<endl;
			cout<<"Producer pid : "<<el.proc_id<<endl;
			cout<<"Priority : "<<el.priority<<endl;
			cout<<"Compute Time : "<<el.cp_time<<endl;
			(*queue).push(el);

			cout<<"Check val : "<<(queue->top()).proc_id<<endl;
		}
		
		//cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		
	}

	
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	
	// shmdt(job_completed);
	// shmdt(cqueue);
	
	pthread_mutex_unlock(lock);

	//pthread_mutex_destroy(lock);
	//shmdt(lock);
	sleep(1);
	if(*job_created<total_jobs)
	{	
		//cout<<"Here"<<endl<<flush;
		//shmdt(job_created);
		producer(id);
		exit(0);
	}
	else
	{
		//shmdt(job_created);
		exit(0);
	}
	exit(0);
}

void consumer(int *id)
{	
	srand(getpid());
	int r = rand()%4;
	sleep(5);

	//key_t key4 = 0x1031;
	const char* key4 = "test5";

	// int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	pthread_mutex_t *lock;
	// lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	int shmid4 = shm_open(key4, O_RDWR, 0666);
	lock = (pthread_mutex_t*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid4, 0);
	// if (pthread_mutex_init(lock, NULL) != 0) { 
 //        printf("\n mutex init has failed\n"); 
 //        return; 
 //    }
    pthread_mutex_lock(lock);
	
	// key_t key1 = 0x1027;
	// key_t key2 = 0x1028;
	// key_t key3 = 0x1025;
	const char* key1 = "test2";
	const char* key2 = "test3";
	const char* key3 = "test4";
	

	// int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	// int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	int shmid1 = shm_open(key1, O_RDWR, 0666);
	int shmid2 = shm_open(key2, O_RDWR, 0666);
	int shmid3 = shm_open(key3, O_RDWR, 0666);
	

	int *job_created,*job_completed;
	

	// job_created = (int*)shmat(id1, NULL, 0);
	// job_completed = (int*)shmat(id2, NULL, 0);

	job_created = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid1, 0);
	job_completed = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid2, 0);
	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *queue;
	queue = (priority_queue<job, vector<job>, Compare_pr>*)mmap(0, 1024, PROT_WRITE, MAP_SHARED, shmid3, 0);
	//priority_queue<job, vector<job>, Compare_pr> qp_val;
	//qp_val = *queue;

	cout<<"Consumer "<<queue->size()<<" "<<(queue->top()).proc_id<<endl;
	
	int sl_time=0;
	if(*job_completed<*job_created)
	{
		if(queue->size()>0)
		{	
			//job el(0,0,0,0,0);
			job el = queue->top();
			//el.cp_time = 4;
			//cout<<"check : "<<((*queue).top()).cp_time<<endl;
			
			

				cout<<"Consumer : "<<*id<<endl;
				cout<<"Consumer pid : "<<getpid()<<endl;
				cout<<"Producer : "<<el.prod_no<<endl;
				cout<<"Producer pid: "<<el.proc_id<<endl;
				cout<<"Priority : "<<el.priority<<endl;
				cout<<"Compute Time : "<<el.cp_time<<endl;
				sl_time = el.cp_time;
				queue->pop();
				*job_completed += 1;
			
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
	
	//shmdt(queue);
	pthread_mutex_unlock(lock);

	// pthread_mutex_destroy(lock);
	
	//shmdt(lock);
	//cout<<"hi"<<flush;
	sleep(sl_time);
	if(*job_completed<*job_created)
	{	
		//cout<<"hi"<<endl<<flush;
		//shmdt(job_completed);
		//shmdt(job_created);
		consumer(id);
		exit(0);
	}
	else
	{
		//shmdt(job_completed);
		//shmdt(job_created);	
		exit(0);
	}
	
	exit(0);
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
	cout<<total_jobs; 

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
	// key_t key1 = 0x1027;
	// key_t key2 = 0x1028;
	// key_t key3 = 0x1025;
	// key_t key4 = 0x1031;

	const char* key1 = "test2";
	const char* key2 = "test3";
	const char* key3 = "test4";
	const char* key4 = "test5";

	// int shmid1 = shmget(key1, 1, IPC_CREAT | 0666);
	// int shmid2 = shmget(key2, 1, IPC_CREAT | 0666);
	// int shmid3 = shmget(key3, 512, IPC_CREAT | 0666);
	// int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

	int shmid1 = shm_open(key1, O_CREAT | O_RDWR, 0666);
	int shmid2 = shm_open(key2, O_CREAT | O_RDWR, 0666);
	int shmid3 = shm_open(key3, O_CREAT | O_RDWR, 0666);
	int shmid4 = shm_open(key4, O_CREAT | O_RDWR, 0666);

	ftruncate(shmid1, 1);
	ftruncate(shmid1, 1);
	ftruncate(shmid1, 1024);
	ftruncate(shmid1, 1); 

	priority_queue<job, vector<job>, Compare_pr> *queue;
	queue = (priority_queue<job, vector<job>, Compare_pr>*)mmap(0, 1024, PROT_WRITE, MAP_SHARED, shmid3, 0);


	int *job_created,*job_completed;
	//cout<<&myseg<<endl;
	// job_created = (int*)shmat(shmid1, NULL, 0);
	// job_completed = (int*)shmat(shmid2, NULL, 0);

	job_created = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid1, 0);
	job_completed = (int*)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid2, 0);

	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(shmid3, NULL, 0);
	//cout<<myseg<<flush;
	//cout<<*myseg<<endl<<flush;
	//int a = 23;	
	pthread_mutex_t *lock;
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init( &attributes );
	pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
	// lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);

	lock = (pthread_mutex_t *)mmap(0, 1, PROT_WRITE, MAP_SHARED, shmid4, 0);

	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    pthread_mutex_lock(lock); 

	*job_created = 0;
	*job_completed = 0; 
	//cout<<*job_created<<" "<<*job_completed<<" "<<endl<<flush;

	
	//queue->push(a);

	// int val1 = shmdt(job_created);
	// int val2 = shmdt(job_completed);
	// int val3 = shmdt(queue);
	// int val4 = shmdt(lock);
	//cout<<val1<<" "<<val2<<" "<<val3<<"\n"<<endl<<flush;

	pthread_mutex_unlock(lock);

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


	pthread_mutex_destroy(lock);
	// shmctl(shmid1, IPC_RMID, NULL);
	// shmctl(shmid2, IPC_RMID, NULL);
	// shmctl(shmid3, IPC_RMID, NULL);
	// shmctl(shmid4, IPC_RMID, NULL);

	shm_unlink(key1);
	shm_unlink(key2);
	shm_unlink(key3);
	shm_unlink(key4); 

	return 0;
	
}