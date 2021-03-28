#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <devices/input.h>

static void syscall_handler (struct intr_frame *);

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

  // arguments to write
  int fd;
  char *buffer;
  unsigned size;

  // arguments to exec
  char *progname;

  switch(syscall_num) {
	case SYS_WRITE:
		// get arguments from stack
		size = *(unsigned*) (f->esp + 4);
		fd = *(int*) (f->esp + 8);
		buffer = (char *) (f->esp + 72);
		printf(buffer);
	break;

	case SYS_EXIT:
		thread_exit ();
	break;

	case SYS_READ:
		c = input_getc();	
		// return the ASCII value of c using EAX
		f->eax = (int) c;
		printf("%c", c);
	break;

	case SYS_EXEC:
		// get the argument froms stack
		progname = *(char **) (f->esp + 4);
		// execute and return the PID using EAX
		f->eax = process_execute(progname);
	break;
  } 
}
