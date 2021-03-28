#include "userprog/process.h"
#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <console.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
static void syscall_handler (struct intr_frame *);
static void sys_exit(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int status = *(esp + 1);
  struct thread *t = thread_current();
  printf("(%s) EXIT : %s\n", t->name, status ? "EXIT_FAILURE" :  "EXIT_SUCCESS");
  thread_exit ();
}

static void sys_exec(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  const char *file = *(esp + 1);
  
  f_->eax = process_execute(file);
}

static void sys_wait(struct intr_frame *f_)
{
}

static void sys_create(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = *(esp + 1);
  unsigned int initial_size = *(esp + 2);
  
  f_->eax = filesys_create(file, initial_size);
}

static void sys_remove(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = *(esp + 1);
  
  f_->eax = filesys_remove(file);
}

static void sys_open(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  char *file = *(esp + 1);
  struct file *f = filesys_open(file);
    
  if (f == NULL) f_->eax = -1;
  else 
  {
    f_->eax = file_insert_in_fd(f);
  }
}

static void sys_filesize(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  struct file *f = file_search_in_fd(fd);
    
  if (f == NULL) f_->eax = -1;
  else f_->eax = file_length(f);  
}

static void sys_read(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  char *buffer = *(esp + 2);
  unsigned int size = *(esp + 3);
  struct file *f;
  
  if (fd == STDIN_FILENO) f_->eax = input_getc();
  else if (fd < 3) f_->eax = -1;
  else
  {
    f = file_search_in_fd(fd);
    if (f == NULL) f_->eax = -1;
    else f_->eax = file_read(f, buffer, size);
  }
}

static void sys_write(struct intr_frame *f_)
{
  unsigned int *esp = f_->esp;
  int fd = *(esp + 1);
  char *buffer = *(esp + 2);
  unsigned int size = *(esp + 3);
  struct file *f;
  
  if (fd == STDOUT_FILENO) putbuf(buffer, size);
  else if (fd < 3) f_->eax = -1;
  else
  {
    f = file_search_in_fd(fd);
    if (f == NULL) f_->eax = -1;
    else f_->eax = file_write(f, buffer, size);
  }
}
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // get the syscall number
  int syscall_num = *(int*) (f->esp);
  uint8_t *ptr = (uint8_t*) (f->esp);
  char c;
	
	unsigned int *esp;
struct file *p;
char *filename;
  // arguments to write
  int fd;
  char *buffer;
  unsigned size;

  // arguments to exec
  char *progname;

  switch(syscall_num) {
	case SYS_WRITE:
		// get arguments from stack
		sys_write(f);
	break;

	case SYS_EXIT:
		sys_exit(f);
	break;

	case SYS_READ:
		sys_read(f);
	break;

	case SYS_EXEC:
		sys_exec(f);

	break;
	case SYS_OPEN:
		sys_open(f);
			break;
  } 
}
