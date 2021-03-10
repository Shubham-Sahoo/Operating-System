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

void producer()
{	
	srand(time(0));
	int r = rand()%4;
	sleep(r);

	key_t key4 = 0x1026;
	int id4 = shmget(key4, 1, IPC_CREAT | 0666);
	pthread_mutex_t *lock;
	lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return; 
    }
    pthread_mutex_lock(lock);

	key_t key1 = 0x1023;
	key_t key2 = 0x1024;
	key_t key3 = 0x1025;
	
	int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	int *seg1,*seg2;
	

	seg1 = (int*)shmat(id1, NULL, 0);
	seg2 = (int*)shmat(id2, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *cqueue;
	cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
	cout<<"Child process "<<endl;
	cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
	//cout<<(&(seg)-0x1)<<endl;

	//int a = 45;
	*seg1 -= 1;
	*seg2 += 1;
	cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
	shmdt(seg1);
	shmdt(seg2);
	shmdt(cqueue);
	shmdt(lock);
	exit(0);
}

void consumer()
{	
	srand(time(0));
	int r = rand()%4;
	sleep(r);

	key_t key4 = 0x1026;
	int id4 = shmget(key4, 1, IPC_CREAT | 0666);
	pthread_mutex_t *lock;
	lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return; 
    }
    pthread_mutex_lock(lock);

	key_t key1 = 0x1023;
	key_t key2 = 0x1024;
	key_t key3 = 0x1025;
	
	int id1 = shmget(key1, 1, IPC_EXCL | 0666);
	int id2 = shmget(key2, 1, IPC_EXCL | 0666);
	int id3 = shmget(key3, 512, IPC_EXCL | 0666);
	
	int *seg1,*seg2;
	

	seg1 = (int*)shmat(id1, NULL, 0);
	seg2 = (int*)shmat(id2, NULL, 0);

	priority_queue<job, vector<job>, Compare_pr> *cqueue;
	cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
	cout<<"Consumer process "<<endl;
	cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
	//cout<<(&(seg)-0x1)<<endl;

	//int a = 45;
	*seg1 -= 1;
	*seg2 += 1;
	cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
	//cout<<*(&seg-1);
	//cout<<(&(seg)-0x1)<<endl;
	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
	shmdt(seg1);
	shmdt(seg2);
	shmdt(cqueue);
	shmdt(lock);
	exit(0);
}

int main()
{	

	int no_p,no_c;
	cout<<"Enter number of producers :"<<endl;
	cin>>no_p;
	cout<<"Enter number of consumers :"<<endl;
	cin>>no_c;

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
	key_t key1 = 0x1023;
	key_t key2 = 0x1024;
	key_t key3 = 0x1025;
	key_t key4 = 0x1026;
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
	lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
	if (pthread_mutex_init(lock, NULL) != 0) { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 

    pthread_mutex_lock(lock); 

	*job_created = 100;
	*job_completed = 25; 
	cout<<*job_created<<" "<<*job_completed<<" "<<endl<<flush;

	pthread_mutex_unlock(lock);

	pthread_mutex_destroy(lock);
	queue->push(a);

	int val1 = shmdt(job_created);
	int val2 = shmdt(job_completed);
	int val3 = shmdt(queue);
	int val4 = shmdt(lock);
	cout<<val1<<" "<<val2<<" "<<val3<<"\n"<<endl<<flush;
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


	for(int i=0;i<no_p;i++)
	{
		if(fork()==0)
		{	
			producer();
			exit(0);
		}
	}

	for(int i=0;i<no_c;i++)
	{
		if(fork()==0)
		{	
			consumer();
			//cout<<"cons process"<<endl;
			exit(0);
		}
	}

	for(int i=0;i<no_p+no_c;i++)
	{
		wait(NULL);
	}

	int pid = fork();

	if(pid==0)
	{
		// int b = stoi("123");
		// cout<<b;
		
		key_t key1 = 0x1023;
		key_t key2 = 0x1024;
		key_t key3 = 0x1025;
		key_t key4 = 0x1026;
		int id1 = shmget(key1, 1, IPC_EXCL | 0666);
		int id2 = shmget(key2, 1, IPC_EXCL | 0666);
		int id3 = shmget(key3, 512, IPC_EXCL | 0666);
		int id4 = shmget(key4, 1, IPC_CREAT | 0666);
		int *seg1,*seg2;
		pthread_mutex_t *lock;
		lock = (pthread_mutex_t*)shmat(id4, NULL, 0);
		if (pthread_mutex_init(lock, NULL) != 0) { 
	        printf("\n mutex init has failed\n"); 
	        return 1; 
	    }
	    pthread_mutex_lock(lock);

		seg1 = (int*)shmat(id1, NULL, 0);
		seg2 = (int*)shmat(id2, NULL, 0);

		priority_queue<job, vector<job>, Compare_pr> *cqueue;
		cqueue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(id3, NULL, 0);
		cout<<*seg1<<" "<<*seg2<<" "<<(cqueue->top()).time<<endl<<flush;
		sleep(2);
		//cout<<(&(seg)-0x1)<<endl;

		//int a = 45;
		*seg1 -= 1;
		*seg2 += 1;
		//cout<<*(&seg-1);
		//cout<<(&(seg)-0x1)<<endl;
		pthread_mutex_unlock(lock);

		pthread_mutex_destroy(lock);
		shmdt(seg1);
		shmdt(seg2);
		shmdt(cqueue);
		shmdt(lock);
		exit(0);

		// key_t key = ftok("shmfile",65); 
  
	 //    shmget returns an identifier in shmid 
	 //    int shmid = shmget(key,1024,0666|IPC_CREAT); 
	  
	 //    shmat to attach to shared memory 
	 //    char *str = (char*) shmat(shmid,(void*)0,0); 
	  
	 //    printf("Data read from memory: %s\n",str); 
	      
	 //    //detach from shared memory  
	 //    shmdt(str); 
	    
	 //    // destroy the shared memory 
	 //    shmctl(shmid,IPC_RMID,NULL); 
	     
	    //return 0; 

	}

	else
	{

		wait(NULL);

		// int pid = fork();

		// if(pid==0)
		// {

		// 	char *a = "jkl";
		// 	myseg = a;
		// 	cout<<string(myseg);
		// 	cout<<&myseg<<endl;
		// 	exit(0);
		// }

		//else
		// {	
		// 	//wait(NULL);
			key_t key1 = 0x1023;
			key_t key2 = 0x1024;
			int id1 = shmget(key1, 1, IPC_EXCL | 0666);
			int id2 = shmget(key2, 1, IPC_EXCL | 0666);
			int *seg1,*seg2;
			seg1 = (int*)shmat(id1, NULL, 0);
			seg2 = (int*)shmat(id2, NULL, 0);
			cout<<*seg1<<" "<<*seg2<<endl<<flush;
			//cout<<string(myseg);
			//cout<<&myseg<<endl;
			shmdt(seg1);
			shmdt(seg2);
			shmctl(shmid1, IPC_RMID, NULL);
			shmctl(shmid2, IPC_RMID, NULL);
			shmctl(shmid3, IPC_RMID, NULL);
			shmctl(shmid4, IPC_RMID, NULL);
			shmctl(id1, IPC_RMID, NULL);
			shmctl(id2, IPC_RMID, NULL);

		// 	return 0;	
		// }
		return 0;	
	}
	return 0;
	
}