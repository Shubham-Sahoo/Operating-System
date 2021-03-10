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
using namespace std;


static int prod_number;

typedef struct job
{
	pid_t proc_id;
	int prod_no;
	int priority;
	int time;
	int job_id;

	job(pid_t proc_id, int prod_no, int priority, int time, int job_id)
        : proc_id(proc_id), prod_no(prod_no),priority(priority),time(time),job_id(job_id)
    {
    }
}job;

struct Compare_pr {
    bool operator()(job const& p1, job const& p2)
    {
        // return "true" if "p1" is ordered 
        // before "p2", for example:
        return p1.priority < p2.priority;
    }
};

void producer(int id)
{	
	srand(time(0));

	int r = rand()%4;
	cout<<"Process "<<getpid()<<" "<<r<<endl;
	sleep(r);

	key_t key4 = 0x1031;
	int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	pthread_mutex_t *lock;
	

	lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return; 
    }
    pthread_mutex_lock(lock);

	key_t key1 = 0x1027;
	key_t key2 = 0x1028;
	key_t key3 = 0x1029;
	
	int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	int *job_created,*job_completed;
	

	job_created = (int*)shmat(id1, NULL, 0);
	job_completed = (int*)shmat(id2, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *cqueue;
	cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
	cout<<"Producer "<<*job_created<<"\t"<<cqueue->size()<<endl;
	//cout<<"hi"<<flush;
	int pr = 1+(rand()%10);
	int j_id = 1+(rand()%100);
	int cmp = 1+(rand()%4);
	
	job el(getpid(),id,pr,cmp,j_id);
	//job el(0,1,3,2,);
	if(*job_created<10)
	{	
		if(cqueue->size()<8)
		{
			cqueue->push(el);
		}
		else
		{
			while(cqueue->size()>=8);
			cqueue->push(el);
		}
		//cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		*job_created += 1;
		cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
	}

	
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	
	shmdt(job_completed);
	shmdt(cqueue);
	
	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
	shmdt(lock);
	if(*job_created<10)
	{	
		//cout<<"Here"<<endl<<flush;
		producer(id);
	}
	shmdt(job_created);
	exit(0);
}

void consumer(int id)
{	
	srand(time(0));
	int r = rand()%4;
	sleep(r);

	key_t key4 = 0x1031;
	int id4 = shmget(key4, 1, IPC_EXCL | 0666);
	pthread_mutex_t *lock;
	lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return; 
    }
    pthread_mutex_lock(lock);

	key_t key1 = 0x1027;
	key_t key2 = 0x1028;
	key_t key3 = 0x1029;
	
	
	int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	int *job_created,*job_completed;
	

	job_created = (int*)shmat(id1, NULL, 0);
	job_completed = (int*)shmat(id2, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *cqueue;
	cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
	cout<<"Consumer "<<*job_completed<<"\t"<<cqueue->size()<<endl;
	job el(0,0,0,0,0);
	
	if(*job_completed<*job_created)
	{
		if(cqueue->size()>0)
		{
			el = cqueue->top();
			cqueue->pop();
			cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
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
			*job_completed -= 1;
		}
		// cout<<*job_created<<" "<<*job_completed<<" "<<el.time<<" "<<id<<endl<<flush;
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		*job_completed += 1;
		

	}

	
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	
	
	shmdt(cqueue);
	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
	
	shmdt(lock);
	//cout<<"hi"<<flush;
	sleep(el.time);
	if(*job_completed<*job_created)
	{	
		//cout<<"hi"<<endl<<flush;
		consumer(id);
	}
	shmdt(job_completed);
	shmdt(job_created);
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
	cin>>no_j; 

	//job p_queue[8];
	priority_queue<job, vector<job>, Compare_pr> p_queue;
	cout<<sizeof(p_queue);
	job a(1,1,5,4,2);
	a.proc_id = 1;
	a.prod_no = 1;
	a.priority = 5;
	a.time = 4;
	a.job_id = 2;
	p_queue.push(a);
	job s = p_queue.top();
	cout<<(s).time<<endl;	
	key_t key1 = 0x1027;
	key_t key2 = 0x1028;
	key_t key3 = 0x1029;
	key_t key4 = 0x1031;
	int shmid1 = shmget(key1, 1, IPC_CREAT | 0666);
	int shmid2 = shmget(key2, 1, IPC_CREAT | 0666);
	int shmid3 = shmget(key3, 512, IPC_CREAT | 0666);
	int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

	int *job_created,*job_completed;
	//cout<<&myseg<<endl;
	job_created = (int*)shmat(shmid1, NULL, 0);
	job_completed = (int*)shmat(shmid2, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *queue;
	queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(shmid3, NULL, 0);
	//cout<<myseg<<flush;
	//cout<<*myseg<<endl<<flush;
	//int a = 23;	
	pthread_mutex_t *lock;
	//pthread_mutexattr_t attributes;
	//pthread_mutexattr_init( &attributes );
	//pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
	lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    pthread_mutex_lock(lock); 

	*job_created = 0;
	*job_completed = 0; 
	cout<<*job_created<<" "<<*job_completed<<" "<<endl<<flush;

	
	//queue->push(a);

	int val1 = shmdt(job_created);
	int val2 = shmdt(job_completed);
	int val3 = shmdt(queue);
	int val4 = shmdt(lock);
	cout<<val1<<" "<<val2<<" "<<val3<<"\n"<<endl<<flush;

	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
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
			producer(prod_number++);

			//exit(0);
		}
	}
	prod_number = 0;
	for(int i=0;i<no_c;i++)
	{
		if(fork()==0)
		{	
			consumer(prod_number++);
			//cout<<"cons process"<<endl;
			//exit(0);
		}
	}

	for(int i=0;i<no_p+no_c;i++)
	{
		wait(NULL);
	}

	// int pid = fork();

	// if(pid==0)
	// {
	// 	// int b = stoi("123");
	// 	// cout<<b;
		
	// 	key_t key1 = 0x1023;
	// 	key_t key2 = 0x1024;
	// 	key_t key3 = 0x1025;
	// 	key_t key4 = 0x1026;
	// 	int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	// 	int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// 	int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	// 	int id4 = shmget(key4, 1, IPC_CREAT | 0666);
	// 	int *seg1,*seg2;
	// 	pthread_mutex_t *lock;
	// 	lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	// 	if (pthread_mutex_init(lock, NULL) != 0) { 
	//         printf("\n mutex init has failed\n"); 
	//         return 1; 
	//     }
	//     pthread_mutex_lock(lock);

	// 	seg1 = (int*)shmat(id1, NULL, 0);
	// 	seg2 = (int*)shmat(id2, NULL, 0);

	// 	priority_queue<job, vector<job>, Compare_pr> *cqueue;
	// 	cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
	// 	cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
	// 	sleep(2);
	// 	//cout<<(&(seg)-0x1)<<endl;

	// 	//int a = 45;
	// 	*seg1 -= 1;
	// 	*seg2 += 1;
	// 	//cout<<*(&seg-1);
	// 	//cout<<(&(seg)-0x1)<<endl;
	// 	pthread_mutex_unlock(lock);

	// 	pthread_mutex_destroy(lock);
	// 	shmdt(seg1);
	// 	shmdt(seg2);
	// 	shmdt(cqueue);
	// 	shmdt(lock);
	// 	exit(0);

	// 	// key_t key = ftok("shmfile",65); 
  
	//  //    shmget returns an identifier in shmid 
	//  //    int shmid = shmget(key,1024,0666|IPC_CREAT); 
	  
	//  //    shmat to attach to shared memory 
	//  //    char *str = (char*) shmat(shmid,(void*)0,0); 
	  
	//  //    printf("Data read from memory: %s\n",str); 
	      
	//  //    //detach from shared memory  
	//  //    shmdt(str); 
	    
	//  //    // destroy the shared memory 
	//  //    shmctl(shmid,IPC_RMID,NULL); 
	     
	//     //return 0; 

	// }

	// else
	// {

	// 	wait(NULL);

	// 	// int pid = fork();

	// 	// if(pid==0)
	// 	// {

	// 	// 	char *a = "jkl";
	// 	// 	myseg = a;
	// 	// 	cout<<string(myseg);
	// 	// 	cout<<&myseg<<endl;
	// 	// 	exit(0);
	// 	// }

	// 	//else
	// 	// {	
	// 	// 	//wait(NULL);
	// 		key_t key1 = 0x1023;
	// 		key_t key2 = 0x1024;
	// 		int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	// 		int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	// 		int *seg1,*seg2;
	// 		seg1 = (int*)shmat(id1, NULL, 0);
	// 		seg2 = (int*)shmat(id2, NULL, 0);
	// 		cout<<*seg1<<" "<<*seg2<<endl<<flush;
	// 		//cout<<string(myseg);
	// 		//cout<<&myseg<<endl;
	// 		shmdt(seg1);
	// 		shmdt(seg2);

	// 		shmctl(id1, IPC_RMID, NULL);
	// 		shmctl(id2, IPC_RMID, NULL);

	// 	// 	return 0;	
	// 	// }
	// 	return 0;	
	// }

	shmctl(shmid1, IPC_RMID, NULL);
	shmctl(shmid2, IPC_RMID, NULL);
	shmctl(shmid3, IPC_RMID, NULL);
	shmctl(shmid4, IPC_RMID, NULL);
	return 0;
	
}