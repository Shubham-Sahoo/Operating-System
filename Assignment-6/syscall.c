#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <devices/shutdown.h>
#include <threads/vaddr.h>
#include <filesys/filesys.h>
#include <string.h>
#include <filesys/file.h>
#include <devices/input.h>
#include <threads/palloc.h>
#include <threads/malloc.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "process.h"
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);

void syscall_halt(void);
void syscall_exit(int *sp);
int syscall_exec(int *sp);
int syscall_wait(int *sp);

int syscall_create(int *sp);
int syscall_remove(int *sp);
int syscall_open(int *sp);
int syscall_filesize(int *sp);
int syscall_read(int *sp);
int syscall_write(int *sp);
void syscall_seek(int *sp);
int syscall_tell(int *sp);
void syscall_close(int *sp);
void is_valid_vaddr(const void *vaddr);
void exit(int exit_code);
struct opened_file *search_file(int fd);

void exit(int exit_code)
{
  thread_current()->exit_code = exit_code;
  thread_exit();
}

void unexpected_exit(){
  exit(-1);
}

void is_valid_vaddr(const void *vaddr)
{
  if (vaddr == NULL)
    unexpected_exit();

  //access kernel page
  if (!is_user_vaddr(vaddr) || !is_user_vaddr(vaddr + 4))
    unexpected_exit();

  //unmapped page
  if (pagedir_get_page(thread_current()->pagedir, vaddr) == NULL)
    unexpected_exit();
}

/*
  get 4 byte information.
  the unit of offset is word(4 byte), or just consider as the arg index in stack sp
*/
void get_stack_elem(int *sp, int *val, int offset)
{
  //note: beware of invalid access
  is_valid_vaddr(sp + offset);
  *val = *(sp + offset);
}

struct opened_file *search_file(int fd)
{
  struct list_elem *elem;
  struct opened_file *of = NULL;
  struct thread *cur = thread_current();

  for (elem = list_begin(&cur->files);
       elem != list_end(&cur->files);
       elem = list_next(elem))
  {
    of = list_entry(elem, struct opened_file, file_elem);
    if (of->fd == fd)
      return of;
  }
  return NULL;
}

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  int *sp = f->esp;
  is_valid_vaddr(sp);

  //stack top is NUMBER
  int syscall_type = (*sp);
  switch (syscall_type)
  {
  case SYS_HALT:
    syscall_halt();
    break;
  case SYS_EXIT:
    syscall_exit(sp);
    break;
  case SYS_EXEC:
    f->eax = syscall_exec(sp);
    break;
  case SYS_WAIT:
    f->eax = syscall_wait(sp);
    break;
  case SYS_CREATE:
    f->eax = syscall_create(sp);
    break;
  case SYS_REMOVE:
    f->eax = syscall_remove(sp);
    break;
  case SYS_OPEN:
    f->eax = syscall_open(sp);
    break;
  case SYS_FILESIZE:
    f->eax = syscall_filesize(sp);
    break;
  case SYS_READ:
    f->eax = syscall_read(sp);
    break;
  case SYS_WRITE:
    f->eax = syscall_write(sp);
    break;
  case SYS_SEEK:
    syscall_seek(sp);
    break;
  case SYS_TELL:
    f->eax = syscall_tell(sp);
    break;
  case SYS_CLOSE:
    syscall_close(sp);
    break;

  default:
    printf("unknown syscall: %d\n", syscall_type);
    break;
  }
}

void syscall_halt(void)
{
  shutdown_power_off();
}

void syscall_exit(int *sp)
{
  int exit_code;
  struct thread *cur = thread_current();
  get_stack_elem(sp, &exit_code, 1);

  cur->exit_code = exit_code;

  thread_exit();
}

int syscall_exec(int *sp)
{
  char *file_name;
  get_stack_elem(sp, &file_name, 1);
  is_valid_vaddr(file_name);

  return process_execute(file_name);
}

int syscall_wait(int *sp)
{
  tid_t child_id;
  get_stack_elem(sp, &child_id, 1);
  return process_wait(child_id);
}

//don't use bool for 32-bit align
int syscall_create(int *sp)
{
  bool success;
  int initial_size;
  char *file_name;

  get_stack_elem(sp, &file_name, 1);
  get_stack_elem(sp, &initial_size, 2);

  is_valid_vaddr(file_name);

  acquire_file_lock();
  success = filesys_create(file_name, initial_size);
  release_file_lock();

  return success;
}
int syscall_remove(int *sp)
{
  bool success;
  char *file_name;

  get_stack_elem(sp, &file_name, 1);
  is_valid_vaddr(file_name);

  acquire_file_lock();
  success = filesys_remove(file_name);
  release_file_lock();
  
  return success;
}

int syscall_open(int *sp)
{
  int fd;
  char *file_name;

  get_stack_elem(sp, &file_name, 1);
  is_valid_vaddr(file_name);

  struct thread *cur = thread_current();

  acquire_file_lock();
  struct file *file = filesys_open(file_name);
  release_file_lock();

  if (file != NULL)
  {
    struct opened_file *of = malloc(sizeof(struct opened_file));
    of->fd = cur->fd_cnt++;
    of->file = file;
    list_push_back(&cur->files, &of->file_elem);
    fd = of->fd;
  }
  else
  {
    fd = -1;
  }

  return fd;
}

int syscall_filesize(int *sp)
{
  int filesize;
  int fd;

  get_stack_elem(sp, &fd, 1);

  struct opened_file *of = search_file(fd);

  if (of != NULL)
  {
    acquire_file_lock();
    filesize = file_length(of->file);
    release_file_lock();
  }
  else
  {
    filesize = -1;
  }
  return filesize;
}

int syscall_read(int *sp)
{
  int readsize;
  int fd;
  char *buffer;
  int size;

  get_stack_elem(sp, &fd, 1);
  get_stack_elem(sp, &buffer, 2);
  get_stack_elem(sp, &size, 3);

  is_valid_vaddr(buffer);

  if (fd == 0)
  {
    for (int i = 0; i < size; i++)
      buffer[i] = input_getc();

    readsize = size;
  }
  else
  {
    struct opened_file *of = search_file(fd);
    if (of != NULL)
    {
      acquire_file_lock();
      readsize = file_read(of->file, buffer, size);
      release_file_lock();
    }
    else
    {
      readsize = -1;
    }
  }
  return readsize;
}
int syscall_write(int *sp)
{
  int writesize;
  int fd;
  void *buffer;
  int size;

  get_stack_elem(sp, &fd, 1);
  get_stack_elem(sp, &buffer, 2);
  get_stack_elem(sp, &size, 3);

  is_valid_vaddr(buffer);

  if (fd == 1)
  {
    putbuf(buffer, size);
    writesize = size;
  }
  else
  {
    struct opened_file *of = search_file(fd);
    if (of != NULL)
    {
      acquire_file_lock();
      writesize = file_write(of->file, buffer, size);
      release_file_lock();
    }
    else
    {
      writesize = 0;
    }
  }

  return writesize;
}
void syscall_seek(int *sp)
{
  int fd;
  int position;

  get_stack_elem(sp, &fd, 1);
  get_stack_elem(sp, &position, 2);

  struct opened_file *of = search_file(fd);

  if (of != NULL)
  {
    acquire_file_lock();
    file_seek(of->file, position);
    release_file_lock();
  }
}
int syscall_tell(int *sp)
{
  int pos;
  int fd;

  get_stack_elem(sp, &fd, 1);

  struct opened_file *of = search_file(fd);
  if (of != NULL)
  {
    acquire_file_lock();
    pos = file_tell(of->file);
    release_file_lock();
  }
  else
  {
    pos = -1;
  }

  return pos;
}

void syscall_close(int *sp)
{
  int fd;
  get_stack_elem(sp, &fd, 1);

  struct opened_file *of = search_file(fd);
  if (of != NULL)
  {
    acquire_file_lock();
    file_close(of->file);
    release_file_lock();

    list_remove(&of->file_elem);
    free(of);
  }
}
