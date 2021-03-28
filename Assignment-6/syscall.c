#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "pagedir.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "filesys/file.h"
#include "userprog/process.h"

static void syscall_handler (struct intr_frame *);
void halt (void);
void exit(int);
int exec (const char *);
int sys_wait (int);
bool remove (const char *);
int sys_open (const char *);
int fileSize (int);
uint32_t read (int, void *, unsigned);
int write (int, const void *, unsigned);
void seek (int, unsigned);
unsigned tell (int);
void close (int);
bool access_user_memory(const void *);
bool create(const char *, unsigned);
bool filedenywrite(struct file *);


int st = 0;


void
syscall_init (void) 
{
  
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

bool access_user_memory(const void *aum){

	
	if(aum == NULL || is_kernel_vaddr(aum) || pagedir_get_page(thread_current ()->pagedir,aum) == NULL){
		//printf("aum is false\n");
		exit(-1);
		return false;
		
		}
		if(!pagedir_get_page(thread_current()->pagedir,aum))
		{
			exit(-1);
			return false;
		}
		return true;

}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

  bool flagfirst = access_user_memory((int *)f->esp);
  int *esp = f->esp;
  int callNum = *(int *)f->esp; 
 

  if(callNum == 0){

	halt();

  }

  else if(callNum == 1){
 	
	bool flag = access_user_memory((int *)f->esp + 1);
	int st = *((int *)f->esp + 1);
	exit(st);
	}
  else if (callNum == 2){
	
	
	bool flag = access_user_memory((int *)f->esp + 1);
	flag = access_user_memory(*((int *)f->esp + 1));
	const char *cmd_line = *((int *)f->esp + 1);
	f->eax = exec (cmd_line);
	

  }
  else if(callNum == 3) {
	int pid = 0;
	bool flag = access_user_memory((int *)f->esp + 1);
	pid = *((int *)f->esp +1);
	f->eax = sys_wait(pid);
  }
  else if(callNum == 4){
	bool flag = false;
	for(int i = 1; i<3;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}

	const char *file = *((int *)f->esp + 1);
	unsigned initial_size = *((int *)f->esp + 2);
	//printf("%s\n",file);
	//printf("%d\n",initial_size);
	flag = access_user_memory(file);
	if(file == NULL)
		exit(-1);

	f->eax =  create(file,initial_size);

	
  }
  else if(callNum == 5){

	bool flag = false;
	for(int i = 1; i<2;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	const char *file = *((int *)f->esp + 1);
	//printf("%s\n",file);
	f->eax = remove(file);

  }
  else if(callNum ==6) {

	bool flag = false;
	//printf("fjnd");
	//hex_dump(f->esp,f->esp,200,true);

	
	//printf("%d\n",file);
	//printf("%s\n",f->esp + 1);
	for(int i = 1; i<2;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	flag = access_user_memory(*((int *)f->esp + 1));
	//printf("fd");
	const char *file = *((int *)f->esp + 1);
        f->eax = sys_open(file);
  }
  else if(callNum == 7) {
	
	int fd = 1;
	bool flag = true;
	for(int i = 1; i<2;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	fd = *((int *)f->esp + 1);
        f->eax = fileSize(fd);
  }
  else if(callNum == 8) {
	
	bool flag = true;
	//hex_dump(f->esp,f->esp,200,true);
		
	for(int i = 1; i<4;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		//printf("i is %d,%x\n",i,(int *)f->esp + i);
		
	}
	//int x = 123;
	int fd = *((int *)f->esp + 1);
	void *buffer = *((int *)f->esp + 2);
	unsigned size = *((int *)f->esp + 3); 
	//printf("fd is %d\n",fd);
	//hex_dump(f->esp,f->esp,200,true);
	flag = access_user_memory(buffer);
	//printf(" buffer is %x\n",buffer);
	//printf("%x\n",&x);
	f->eax = read (fd, buffer, size);

  }
  else if(callNum == 9){


	bool flag = true;
		
	for(int i = 1; i<4;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		//printf("%x\n",(int *)f->esp + i);
		//printf("%x\n",*((int *)f->esp + 8));
		
	}
		//flag = access_user_memory(*((int *)f->esp + 2));
		int fd = *((int *)f->esp + 1);
		char *buf = *((int *)f->esp + 2);
		unsigned size = *((int *)f->esp + 3);
		//printf("%x\n",buf);
		//sema_down(&filesync);
		f->eax = write (fd, buf, size);
	
		
	}
  else if(callNum == 10){
	
	int fd = 0;
	unsigned position = 0;
	bool flag = true;
		
	for(int i = 1; i<3;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	fd = *((int *)f->esp + 1);
	position = *((int *)f->esp +2);
	seek (fd, position);


  }
  else if(callNum == 11){
	
	int fd = 0;
	bool flag = true;
		
	for(int i = 1; i<2;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	fd = *((int *)f->esp + 1);
	tell(fd);
	//printf("ndf\n");




  }
  else if(callNum == 12){
	
	//int fd =1;
	bool flag = true;
		
	for(int i = 1; i<2;i++){
		
		flag = access_user_memory((int *)f->esp + i);
		
	}
	//flag = access_user_memory((int *)f->esp + 2);
	//printf("%d\n",*((int *)f->esp + 1));
	close(*((int *)f->esp + 1));

  }
	
}


void halt (){


	shutdown_power_off();

}


void exit(int status){
	
	if(status == -1)
		st = -1;
	struct thread *t = thread_current();
        struct list_elem *exr;
	if(t->parentID != 0 )
	{
		//printf("partID is ther\n");
		updateStatus(t->parentID,status);
	}
	//printf("size is in exit %d\n",list_size(&files));
	struct filefd *g = list_entry(list_begin(&files),struct filefd,e);
        //printf("tid is %d\n",g->tid);
        isEdit(g->f);
	sema_up(&ex);
	//sema_up(&ex);
	printf("%s: exit(%d)\n",thread_current()->name,status);
	//sema_down(&ex);
	thread_exit();
}


int exec (const char *cmd_line){
	
	//printf("In exec fun\n");
	char *gf,*token,*save_ptr;
	gf = palloc_get_page(0);
	strlcpy(gf, cmd_line, PGSIZE);
	
	const char *of = strtok_r(gf," ", &save_ptr);
	enum intr_level old_level;
	int ret = process_execute(cmd_line);
	
	if(st == -1)
		return -1;
	
	return ret;
}

bool create(const char *file, unsigned initial_size){

	sema_down(&filesync);
	bool ret = filesys_create(file,initial_size);
	sema_up(&filesync);

return ret;
}


int sys_wait (int pid) {

	//printf("In sys_wait\n");
	struct list_elem *tyu;
	struct thread *t = thread_current();
	for(tyu = list_begin(&t->childs);
			tyu != list_end(&t->childs); tyu = list_next(tyu))
			{
				struct child *cu = list_entry(tyu,struct child,e);
				//printf("%d\n",cu->childstatus);
				//if(cu->childstatus!=0)
				//	return -1;
				if(cu->threadid == pid && !cu->fl){
					cu->fl = true;
					return cu->childstatus;
				}
				else
					return -1;
			}
		//printf("In sys_wait1\n");
	return -1;
}




bool remove (const char *file){

	//file = "";
	sema_down(&filesync);
	bool r = filesys_remove(file);
	sema_up(&filesync);
	return r;
}




int sys_open (const char *file){

	int ret = -1;

	if(file == NULL)
		exit(-1);

        sema_down(&filesync);
	struct file *f = filesys_open(file);
	sema_up(&filesync);
	//f->deny_write = true;
	
	if(f == NULL)
	{
		sema_up(&filesync);
		return ret;
	}
	
	
	//printf("%d\n",thread_current()->maxfd);
	//if(thread_current()->maxfd == 0)
	//	thread_current()->maxfd = 1;
	else
	{
		struct filefd *f1 = calloc(1,sizeof *f1);
		struct thread *ot = malloc(sizeof(struct thread));
		ret = thread_current ()->maxfd;
		
		f1->fd = ret; 
		f1->fdt = thread_current();
		thread_current ()->maxfd = thread_current()->maxfd + 1;
		f1->f = f;
		f1->tid = thread_current()->tid;
		//printf("kfjsk\n");
		list_push_back(&files,&f1->e);
		//printf("size is %d\n",list_size(&files));
	}
	//printf("size of file list is %d\n",list_size(&thread_current()->files));
	
	
	
	//printf("%d\n",thread_current ()->maxfd);
	
	return ret;
}


int fileSize (int fd){

	
	struct thread *t = thread_current();
	struct list_elem *e1;
	int filesize = 0;
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){
	
		struct filefd *fed = list_entry(e1,struct filefd,e);
			
		if(fed->fd == fd && t->tid == fed->tid){

			sema_down(&filesync);
			filesize = file_length(fed->f);
			sema_up(&filesync);
		}

	}
	//printf("file size in syscal.c %d\n",filesize);
	return filesize;
}


uint32_t read (int fd, void *buffer, unsigned size){

	uint32_t x = 0;
	if(fd == 0)
	{
		buffer = input_getc();
		return size;
	}
	else if(fd == 1)
	{
		exit(-1);
		return size;
	}
	else
	{
	struct thread *t = thread_current();
	struct list_elem *e1;
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){

		struct filefd *fed = list_entry(e1,struct filefd,e);
		if(fed->fd == fd && t->tid == fed->tid)
		{
			sema_down(&filesync);
			//printf("bfjdnbfd\n");
			struct file *f1 = fed->f;
			//printf("file pos is at %jd\n",f1->pos);
			x = file_read(fed->f,buffer,size);
			size = x;
			//printf("x is %d\n",x);
			sema_up(&filesync);
		}
	}
	
	}
	
	return x;
}


int write (int fd, const void *buffer, unsigned size){

	//printf("%x\n",buffer);
	//(int **)
	bool flag = access_user_memory(buffer);
	
	if(fd == 1){
		//printf("%s\n",buffer);
		
		putbuf(buffer,size);
		
	}
	else if(fd == 0){
	//printf("fd is 0");
		exit(-1);	
	}
	else
	{
	struct thread *t = thread_current();
	struct list_elem *e1;
	//printf("njf\n");
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){

		struct filefd *fed = list_entry(e1,struct filefd,e);

		bool deny = isEdit(fed->f);
		//printf("%d and tid is %d,%s\n",deny,fed->tid,fed->fdt->name);
		if(fed->fd == fd && t->tid == fed->tid)
		{
			//struct file *f1 =  &(fed->f);
			//f1->deny_write = false;
			
			//printf("deny %d\n",deny);
			//if(deny){
			//printf("run\n");
			sema_down(&filesync);
			int x = file_write(fed->f,buffer,size);
			//printf("after write %d\n",x);
			size = x;
			//printf("%d\n",x);
			sema_up(&filesync);
			//}
		}
	}
				
	}
	//printf("bdhghf\n");
	
	return (int)size;
}


void seek (int fd, unsigned position){

	struct thread *t = thread_current();
	struct list_elem *e1;
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){
	
		struct filefd *fed = list_entry(e1,struct filefd,e);
			
		if(fed->fd == fd && t->tid == fed->tid){

			sema_down(&filesync);
			file_seek(fed->f,position);
			sema_up(&filesync);
		}

	}
}


unsigned tell (int fd){

	struct thread *t = thread_current();
	struct list_elem *e1;
	int ret = 0;
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){
	
		struct filefd *fed = list_entry(e1,struct filefd,e);
			
		if(fed->fd == fd && t->tid == fed->tid){

			sema_down(&filesync);
			ret = file_tell(fed->f);
			sema_up(&filesync);
		}

	}
	return ret;
}




void close (int fd){
	
	if(fd == 0 || fd == 1)
		exit(-1);
	struct thread *t = thread_current();
	struct list_elem *e1;
	for(e1 = list_begin(&files); e1 != list_end(&files);
	    e1 = list_next(e1)){
	
		struct filefd *fed = list_entry(e1,struct filefd,e);
			
		if(fed->fd == fd && t->tid == fed->tid){

			sema_down(&filesync);
			file_close(fed->f);
			list_remove(e1);
			sema_up(&filesync);
		}

	}
	
}
