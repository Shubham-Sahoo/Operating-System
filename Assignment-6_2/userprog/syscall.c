#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"

#define BOTTOM_VADDR ((void *) 0x08048000)


static void syscall_handler (struct intr_frame *);

void get_arg (struct intr_frame *, int *, int);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init(&syscall_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf("check - f->esp = %p\n\n", f->esp);

  int args[7];

  //validate pointer
  if (!(is_user_vaddr((const void*) f->esp) == true && (const void*) f->esp > BOTTOM_VADDR)) {
    exit(-1);
  }
  
  if (f->esp == 0x20101234) {
    exit(-1);
  }
  
  switch (*((int *) f->esp)) {


  	case SYS_EXIT: {

      get_arg(f, &args[0], 1);
      //printf("status:::::: %d\n", args[0]);
  		exit(args[0]);
		//break;
		// exit(0);
		 break;
  	}
  	case SYS_WRITE: {
      
      get_arg(f, &args[0], 6);
      int size = (f->esp);
      // for (int i = 0; i < 19; ++i)
      // {
      //   /* code */
      //   if(args[i]>0x08048000)
      //     printf("Hi %d %s \n",i, args[i]);
      // }
      
      // printf("Hi here %s\n", args[5]);
 	    buffer_check((void *) (args[5]), (unsigned) (args[0]));
      // printf("Hi here %s\n", args[5]);
      args[5] = ptr_to_kernel_mode((const void *) (args[5]));

      f->eax = write(args[1], (const void *) args[5], (unsigned) args[0]);
      break;
  	}
  	case SYS_WAIT: {
  		get_arg(f, &args[0], 1);
  		f->eax = wait(args[0]);
  		break;
  	}
  	case SYS_HALT: {
  		halt();
  		break;
  	}
  	case SYS_EXEC: {
      get_arg(f, &args[0], 1);
      args[0] = ptr_to_kernel_mode((const void *) args[0]);
      f->eax = exec((const void *) args[0]);
      break;
  	}
  	case SYS_CREATE: {
      get_arg(f, &args[0], 2);
      args[0] = ptr_to_kernel_mode((const void *) args[0]);
      f->eax = create((const char *) args[0], (unsigned) args[1]);
      break;

  	}
  	case SYS_REMOVE: {
      get_arg(f, &args[0], 1);
      args[0] = ptr_to_kernel_mode((const void *) args[0]);
      f->eax = remove((const char *) args[0]);
      break;
  	} 
  	case SYS_OPEN: {
  		get_arg(f, &args[0], 1);
  		args[0] = ptr_to_kernel_mode((const void *) args[0]);
  		f->eax = open((const char *) args[0]);
  		break; 
  	}
  	case SYS_FILESIZE: {
  		get_arg(f, &args[0], 1);
		f->eax = filesize(args[0]);
		break;
  	}
  	case SYS_READ: {
  		get_arg(f, &args[0], 3);

  		//check if the buffer is valid by checking each ptr in the buff
  		buffer_check((void *) args[1], (unsigned) args[2]);

  		args[1] = ptr_to_kernel_mode((const void *) args[1]);
  		f->eax = read(args[0], (void *) args[1], (unsigned) args[2]);
  		break;
  	}
  	case SYS_SEEK: {
      get_arg(f, &args[0], 2);
      seek(args[0], (unsigned) args[1]);
      break;

  	}
  	case SYS_TELL: {
      
      get_arg(f, &args[0], 1);
      f->eax = tell(args[0]);
      break;
      
  	}
  	case SYS_CLOSE: {
  		get_arg(f, &args[0], 1);
		  close(args[0]);
		  break;
  	}
  }


  //printf ("system call!\n");
}

int write(int fd, const void* buffer, unsigned size) 
{

	lock_acquire(&syscall_lock);

	if (fd==1) {
	  putbuf((char*)buffer, size);
	  lock_release(&syscall_lock);
	  return size;
	} 


    if(!list_empty(&thread_current()->file_list)) {
      struct list_elem* iter = NULL;
      for (iter = list_begin(&thread_current()->file_list); iter != list_end(&thread_current()->file_list);
          iter = list_next(iter)) 
      {
          struct process_file* pf = list_entry(iter, struct process_file, elem);
          if (pf->fd == fd) {
            struct file* f = pf->file;
            int retVal = file_write (f, buffer, size); 
            lock_release(&syscall_lock);          
            return retVal;
          }
      }
    }

    //3. Return num bytes written
    lock_release(&syscall_lock);          
    return -1;
}

void exit (int status){
  //printf("status:::::: %d\n", status);

  printf ("%s: exit(%d)\n", thread_current()->name, status);

  struct thread *cur = thread_current();
  struct thread *par = thread_current()->parent_thread;

  if (thread_alive(cur->tid)) {
    if (par != NULL) {

      for (struct list_elem *e = list_begin (&par->child_obituaries); e != list_end (&par->child_obituaries); e = list_next (e)) {
        struct child_obituary *co = list_entry (e, struct child_obituary, dead_child_elem);
        if (co->pid == cur->tid) {
          co->exit_status = status;
          co->exited = true;
          sema_up(&co->process_wait_sema);
        }
      }

    } 
  }


  thread_exit();
}

void halt(void) {
	shutdown_power_off();
}

pid_t exec(const char* cmd_line) 
{ 
	const char* copy_of_cmd_line = cmd_line; 
   pid_t pid = process_execute(cmd_line);

   if (pid == -1) {
    return -1;
   }
  
   return pid;
}

int wait(pid_t pid) 
{ 
  int exit_status = process_wait(pid);

  return exit_status;
}

bool create (const char* file, unsigned initial_size) 
{
  lock_acquire(&syscall_lock);
  bool status = filesys_create(file, initial_size);
  lock_release(&syscall_lock);
  return status;
}

bool remove(const char* file) {
  lock_acquire(&syscall_lock);
  bool remove_file_ok = filesys_remove(file);
  lock_release(&syscall_lock);          
  return remove_file_ok;
}

int open(const char* file) {
	
    lock_acquire(&syscall_lock);

    struct file *fo = filesys_open(file);
    if (fo == NULL) {
    	lock_release(&syscall_lock);
    	return -1;
    }

    struct process_file *pf = malloc(sizeof(struct process_file));
    pf->file = fo;
    pf->fd = thread_current()->fd;
    thread_current()->fd++;
    list_push_back(&thread_current()->file_list, &pf->elem);
    int fp = pf->fd;

    lock_release(&syscall_lock);
    return fp;
}

int filesize(int fd) {
  lock_acquire(&syscall_lock);
  struct file *fo = NULL;
  for (struct list_elem* e = list_begin (&thread_current()->file_list); e != list_end (&thread_current()->file_list); e = list_next (e)) {
    struct process_file *pf = list_entry (e, struct process_file, elem);
    if (fd == pf->fd) {
	  fo = pf->file;
	}
  }

  if (!fo) {
    lock_release(&syscall_lock);
    return -1;
  }
  int filesize = file_length(fo);
  lock_release(&syscall_lock);  
  return filesize;
}

int read(int fd, void* buffer, unsigned size) {
  lock_acquire(&syscall_lock);

  if (fd == STDIN_FILENO) {
    uint8_t* buff = (uint8_t *) buffer;
    for (uint8_t i = 0; i < size; i++) {
	  buff[i] = input_getc();
	}
	  lock_release(&syscall_lock);
      return size;
  }


  struct file *fo = NULL;
  for (struct list_elem* e = list_begin (&thread_current()->file_list); e != list_end (&thread_current()->file_list); e = list_next (e)) {
    struct process_file *pf = list_entry (e, struct process_file, elem);
    if (fd == pf->fd) {
      fo = pf->file;
    }
  }

  if (!fo) {
  	lock_release(&syscall_lock);          
    return -1;
  }
  int size_bytes = file_read(fo, buffer, size);
  lock_release(&syscall_lock);
  return size_bytes;
}

void seek(int fd, unsigned position) {
  lock_acquire(&syscall_lock);

  struct file *f = NULL;
  for (struct list_elem* e = list_begin(&thread_current()->file_list); e != list_end(&thread_current()->file_list); e = list_next(e)) {
    struct process_file *pf = list_entry(e, struct process_file, elem);
    if (fd == pf->fd) {
      f = pf->file;
    }
  }

  if (f == NULL) {
    lock_release(&syscall_lock);
    return;
  }

  file_seek(f, position);
  lock_release(&syscall_lock);
}

unsigned tell(int fd) {
  lock_acquire(&syscall_lock);

  struct file *f = NULL;
  for (struct list_elem* e = list_begin(&thread_current()->file_list); e != list_end(&thread_current()->file_list); e = list_next(e)) {
    struct process_file *pf = list_entry(e, struct process_file, elem);
    if (fd == pf->fd) {
      f = pf->file;
    }
  }

  if (f == NULL) {
    lock_release(&syscall_lock);
    return -1;
  }

  off_t result = file_tell(f);
  lock_release(&syscall_lock);
  return result;
}

void close(int fd) {
  lock_acquire(&syscall_lock);

  struct list_elem *next, *e = list_begin(&thread_current()->file_list);

  while (e != list_end (&thread_current()->file_list)) {
    next = list_next(e);
    struct process_file *pf = list_entry (e, struct process_file, elem);
    if (fd == pf->fd || fd == -1) {
  	  file_close(pf->file);
  	  list_remove(&pf->elem);
  	  free(pf);
  	  if (fd != -1) {
        lock_release(&syscall_lock);
  	    return;
  	  }
	}
    e = next;
  }

  lock_release(&syscall_lock);
  return;
}


int ptr_to_kernel_mode(const void* vaddr) {

  if (!is_user_vaddr(vaddr) || vaddr < BOTTOM_VADDR) {
    exit(-1);
  }
  void *ptr = pagedir_get_page(thread_current()->pagedir, vaddr);
  if (!ptr) {
    exit(-1);
  }
  return (int) ptr;
}

int buffer_check(const void* buffer_vaddr, unsigned size) {
  
  // printf("Hi sys\n");
  if (!is_user_vaddr(buffer_vaddr) ||  buffer_vaddr < BOTTOM_VADDR) 
  {
    // printf("Hi sys\n");
    exit(-1);
  }
  
  if (!is_user_vaddr(buffer_vaddr + size) ||  (buffer_vaddr + size) < BOTTOM_VADDR) 
  {
    // printf("Hi sys\n");
    exit(-1);
  }
  void *ptr = pagedir_get_page(thread_current()->pagedir, buffer_vaddr);
  if (!ptr) 
  {
    
    exit(-1);
  }
  // printf("Hi sys\n");
  return (int) ptr;
}

//helper fn to parse out the argument for the corresponding syscall
void get_arg (struct intr_frame *f, int *args, int num_args) {
  int *ptr;
  for (int i = 0; i < num_args; i++) {
    ptr = (int *) f->esp + i + 1;
    
    if (is_user_vaddr((const void*) ptr) == false || (const void*) ptr < BOTTOM_VADDR) {
    	exit(-1);
    } 

    
    if (ptr == NULL) {
      exit(-1);
    }

    
    args[i] = *ptr;
    //printf("args[%d]: %d   ", i, args[i]);
  }
}