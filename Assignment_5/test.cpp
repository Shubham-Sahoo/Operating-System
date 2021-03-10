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

int main()
{	
	
	key_t key1 = 0x1023;
	key_t key2 = 0x1024;
	key_t key3 = 0x1025;
	key_t key4 = 0x1026;
	int shmid1 = shmget(key1, 1, IPC_CREAT | 0666);
	int shmid2 = shmget(key2, 1, IPC_CREAT | 0666);
	//int shmid3 = shmget(key3, 512, IPC_CREAT | 0666);
	int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

	int *job_created,*job_completed;
	//cout<<&myseg<<endl;
	job_created = (int*)shmat(shmid1, NULL, 0);
	job_completed = (int*)shmat(shmid2, NULL, 0);

	//cout<<myseg<<flush;
	//cout<<*myseg<<endl<<flush;
	//int a = 23;
	pthread_mutex_t *lock;
	pthread_mutexattr_t attributes;
	pthread_mutexattr_init( &attributes );
	pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
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
	int val4 = shmdt(lock);
	cout<<val1<<" "<<val2<<" "<<val4<<"\n"<<endl<<flush;

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

	int pid = fork();
	if(pid==0)
	{	
		key_t key1 = 0x1023;
		key_t key2 = 0x1024;
		key_t key3 = 0x1025;
		key_t key4 = 0x1026;
		int shmid1 = shmget(key1, 1, IPC_CREAT | 0666);
		int shmid2 = shmget(key2, 1, IPC_CREAT | 0666);
		//int shmid3 = shmget(key3, 512, IPC_CREAT | 0666);
		int shmid4 = shmget(key4, 1, IPC_CREAT | 0666);

		int *job_created,*job_completed;
		//cout<<&myseg<<endl;
		job_created = (int*)shmat(shmid1, NULL, 0);
		job_completed = (int*)shmat(shmid2, NULL, 0);
		pthread_mutex_t *lock;
		pthread_mutexattr_t attributes;
		pthread_mutexattr_init( &attributes );
		pthread_mutexattr_setpshared( &attributes, PTHREAD_PROCESS_SHARED );
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
		int val4 = shmdt(lock);
	}
	else
	{
		wait(NULL);
	}
	return 0;
}