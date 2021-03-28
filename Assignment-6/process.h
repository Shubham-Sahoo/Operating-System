#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct child {

	struct list_elem e;
	struct thread *t;
	int childstatus;
	int threadid;
	bool fl;
};

struct filefd {

	int fd;
	struct list_elem e;
	struct file *f;
	int tid;
	struct thread *fdt;

};

//static void start_process (void *);

#endif /* userprog/process.h */
