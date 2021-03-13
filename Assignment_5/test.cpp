#include <iostream> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include <sys/types.h>
#include<sys/wait.h>  
#include <unistd.h> 
#include <string.h>
#include <queue>
#include <pthread.h>
#include <mutex> 
#include <time.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/mman.h>

using namespace std;

typedef struct job
{
	pid_t proc_id;
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


struct SHM
{
	priority_queue<job,vector<job>,Compare_pr> prq;
    int job_created;
    int job_completed;
}memory;


void cons()
{
	const int SIZE = 4096; 
  
    /* name of the shared memory object */
    const char* name = "test2"; 
  
    /* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory object */
    //int* ptr; 
  
    /* open the shared memory object */
    shm_fd = shm_open(name, O_RDWR, 0666); 
  
    /* memory map the shared memory object */
    //ptr = (int*)mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0); 
  
    /* read from the shared memory object */
    cout<<"Hi"<<flush;
    //printf("%s", (char*)ptr); 
    //cout<<*ptr<<flush;
  
    priority_queue<job, vector<job>, Compare_pr> *pq;
	pq = (priority_queue<job, vector<job>, Compare_pr>*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	job s;
	s.proc_id = 41;
	//cout<<"Here";
	pq->push(s);

	cout<<"Value : "<<(pq->top()).proc_id<<endl<<flush; 
	pq->pop();
	cout<<"Value : "<<(pq->top()).proc_id<<endl<<flush; 

    /* remove the shared memory object */
    shm_unlink(name); 
    exit(0);
}

int main()
{	
	
	// key_t key1 = 0x1005;
	// key_t key2 = 0x1006;
	// key_t key3 = 0x1007;
	// key_t key4 = 0x1004;
	// int shmid1 = shmget(key1, 1, IPC_CREAT | 0666);
	// int shmid2 = shmget(key2, 1, IPC_CREAT | 0666);
	// int shmid3 = shmget(key3, 512, IPC_CREAT | 0666);
	// int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

	const char* name = "test2";
	const int SIZE = 4096; 

	const char* message_0 = "Hello"; 
    const char* message_1 = "World!"; 

	/* shared memory file descriptor */
    int shm_fd; 
  
    /* pointer to shared memory obect */
    int* ptr; 
  
    /* create the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); 
  
    /* configure the size of the shared memory object */
    ftruncate(shm_fd, SIZE); 
  
    /* memory map the shared memory object */
    // ptr = (int*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); 
  
    // /* write to the shared memory object */
    // //sprintf(ptr, "%d", 2); 
    // *ptr = 2;
  
    // ptr += 1;//strlen(message_0); 
    // //sprintf(ptr, "%d", 3);
    // *ptr = 3; 
    // ptr -= 1;//strlen(message_1); 

    //cout<<"Hi here : "<<*ptr<<flush;

	priority_queue<job, vector<job>, Compare_pr> *pq;
	pq = (priority_queue<job, vector<job>, Compare_pr>*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

	job s;
	s.proc_id = 22;

	pq->push(s);

	cout<<"Value : "<<(pq->top()).proc_id<<endl<<flush; 





	// cout<<sizeof(p_queue);
	// job a(1,1,5,4,2);
	// a.proc_id = 1;
	// a.prod_no = 1;
	// a.priority = 5;
	// a.cp_time = 10;
	// a.job_id = 2;
	// p_queue.push(a);
	// job s = p_queue.top();
	// cout<<(s).cp_time<<endl;

	// int *job_created,*job_completed;
	// //cout<<&myseg<<endl;
	// job_created = (int*)shmat(shmid1, NULL, 0);
	// job_completed = (int*)shmat(shmid2, NULL, 0);

	// priority_queue<job, vector<job>, Compare_pr> *queue;
	// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(shmid3, NULL, 0);
	

	// //cout<<myseg<<flush;
	// //cout<<*myseg<<endl<<flush;
	// //int a = 23;
	// pthread_mutex_t *lock;
	// lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
	// cout<<"Here"<<flush;
	// pthread_mutexattr_t attributes;
	// pthread_mutexattr_init( &attributes );
	// pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
	
	// // if (pthread_mutex_init(lock, NULL) != 0) { 
 // //        printf("\n mutex init has failed\n"); 
 // //        return 1; 
 // //    } 

 //    pthread_mutex_lock(lock); 

	// *job_created = 0;
	// *job_completed = 0; 
	// cout<<*job_created<<" "<<*job_completed<<" "<<endl<<flush;
	// queue->push(a);
	// a.priority = 10;
	// a.cp_time = 20; 
	// queue->push(a);
	
	// //queue->push(a);

	// int val1 = shmdt(job_created);
	// int val2 = shmdt(job_completed);
	// int val4 = shmdt(lock);
	// cout<<val1<<" "<<val2<<" "<<val4<<"\n"<<endl<<flush;

	// pthread_mutex_unlock(lock);

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

	//int pid = fork();
	if(fork()==0)
	{	

		cons();
		// key_t key1 = 0x1005;
		// key_t key2 = 0x1006;
		// key_t key3 = 0x1007;
		// key_t key4 = 0x1004;
		// int shmid1 = shmget(key1, 1, IPC_EXCL | 0666);
		// int shmid2 = shmget(key2, 1, IPC_EXCL | 0666);
		// int shmid3 = shmget(key3, 512, IPC_EXCL | 0666);
		// int shmid4 = shmget(key4, 1, IPC_EXCL | 0666);

		// int *job_created,*job_completed;
		// //cout<<&myseg<<endl;
		// cout<<"There"<<flush;
		// job_created = (int*)shmat(shmid1, NULL, 0);
		// job_completed = (int*)shmat(shmid2, NULL, 0);

		// priority_queue<job, vector<job>, Compare_pr> *queue;
		// queue = (priority_queue<job, vector<job>, Compare_pr> *)shmat(shmid3, NULL, 0);

		// pthread_mutex_t *lock;
		// lock = (pthread_mutex_t*)shmat(shmid4, NULL, 0);
		// cout<<"There"<<flush;
		// pthread_mutexattr_t attributes;
		// pthread_mutexattr_init( &attributes );
		// pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
		
		// // if (pthread_mutex_init(lock, NULL) != 0) { 
	 // //        printf("\n mutex init has failed\n"); 
	 // //        return 1; 
	 // //    } 

	 //    pthread_mutex_lock(lock); 

		// *job_created = 0;
		// *job_completed = 0; 
		// job el = queue->top();
		// cout<<*job_created<<" "<<*job_completed<<" "<<el.cp_time<<endl<<flush;
		// queue->pop();
		// el = queue->top();
		// cout<<*job_created<<" "<<*job_completed<<" "<<el.cp_time<<endl<<flush;

		
		// //queue->push(a);

		// int val1 = shmdt(job_created);
		// int val2 = shmdt(job_completed);
		// int val4 = shmdt(lock);
	}

	wait(NULL);

	if(fork()==0)
	{
		cons();
	}

	wait(NULL);

	// else
	// {
	// 	wait(NULL);

	// 	// shmctl(shmid1, IPC_RMID, NULL);
	// 	// shmctl(shmid2, IPC_RMID, NULL);
	// 	// shmctl(shmid3, IPC_RMID, NULL);
	// 	// shmctl(shmid4, IPC_RMID, NULL);
	// }

	return 0;
}