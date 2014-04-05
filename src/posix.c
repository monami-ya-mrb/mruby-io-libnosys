/*
 * Copyright (c) 2014 monami-ya.mrb developers

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
DEALINGS IN THE SOFTWARE.

*/
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ff.h"
#include "mruby.h"
#include "mruby-io-libnosys/config.h"
#include "mruby-io-libnosys/extern.h"

#undef errno
extern int errno;

FATFS fatfs[MRUBY_IO_LIBNOSYS_DRIVE_MAX];

typedef struct file_desc_t_ {
  struct file_desc_t_ *next; /* NULL means in use */
  FIL fil;
  int posix_fd;
} file_desc_t; 
file_desc_t file_desc[MRUBY_IO_LIBNOSYS_FD_MAX];

file_desc_t file_pool;

/* As 0,1,2 is kept by stdin/stdout/stderr */
#define POSIX_FD_TO_INTERNAL(fd) ((fd) - 3)
#define INTERNAL_TO_POSIX_FD(fd) ((fd) + 3)

static void
init_file_desc(void)
{
  file_pool.next = &file_pool;
}

static file_desc_t *
pop_file_desc(void)
{
  file_desc_t *p_result;

  if (file_pool.next != &file_pool) {
    p_result = file_pool.next;
    file_pool.next = file_pool.next->next;
    p_result->next = NULL;
  } else {
    p_result = NULL;
  }

  return p_result;
}

static void
push_file_desc(file_desc_t *file_desc)
{
  file_desc->next = file_pool.next;
  file_pool.next = file_desc;
}

static inline int
is_opened(file_desc_t *p_file_desc)
{
  return p_file_desc->next == NULL;
}


extern int printk(const char *, ...);
int
mrb_mruby_io_libnosys_open(const char *name, int posix_flags, ...) {
  file_desc_t *p_file_desc;
  FRESULT fresult;
  int fatfs_flags = 0;
  int fd_status;
  int do_append = 0;
  TCHAR wcname[512];

  {
    /* TODO: consider multi byte chars. */
    char *p;
    int i;
    for (i = 0, p = (char *)name; *p != '\0'; p++, i++) {
      wcname[i] = (TCHAR)*p;
    }
    wcname[i] = 0;
  }
  printk("open(%s, %d)", wcname, posix_flags);
  p_file_desc = pop_file_desc();
  if (p_file_desc) {

    if ((posix_flags & O_ACCMODE) == O_RDWR) {
      fatfs_flags |= FA_READ | FA_WRITE;
    } else if ((posix_flags & O_ACCMODE) == O_RDONLY) {
	fatfs_flags |= FA_READ;
    } else if ((posix_flags & O_ACCMODE) == O_WRONLY) {
      fatfs_flags |= FA_WRITE;
    }

    if ((posix_flags & O_APPEND) == O_APPEND) {
      do_append = 1;
    }
    if ((posix_flags & O_CREAT) == O_CREAT) {
      fatfs_flags |= FA_OPEN_ALWAYS;
    }
    if ((posix_flags & O_TRUNC) == O_TRUNC) {
      fatfs_flags |= FA_CREATE_ALWAYS;
    }

    fresult = f_open(&(p_file_desc->fil), wcname, fatfs_flags);
    if (fresult == FR_OK) {
      fd_status = p_file_desc->posix_fd;
      if (do_append) {
	(void) f_lseek((&p_file_desc->fil), f_size(&(p_file_desc->fil)));
      }
    } else {
      push_file_desc(p_file_desc);
      fd_status = -1;
      /* TODO: set errno */ 
    } 
  } else {
    /* file dscriptor exhausted. */
    fd_status = -1;
    errno = ENOMEM;
  }

  return fd_status;
}

int
mrb_mruby_io_libnosys_close(int posix_fd)
{
  int internal_fd = POSIX_FD_TO_INTERNAL(posix_fd);
  file_desc_t *p_file_desc = &file_desc[internal_fd];
  FRESULT fresult;
  int status;

  if (is_opened(p_file_desc)) {
    fresult = f_close(&(p_file_desc->fil));
    if (fresult == FR_OK) {
      status = 0;
      push_file_desc(p_file_desc);
    } else {
      status = -1;
      /* TODO: errno */
    }
  } else {
    errno = EBADF;
    status = -1;
  }

  return status;
}

ssize_t
mrb_mruby_io_libnosys_read(int posix_fd, void *buf, size_t len)
{
  int internal_fd = POSIX_FD_TO_INTERNAL(posix_fd);
  file_desc_t *p_file_desc = &file_desc[internal_fd];
  UINT rlen;
  FRESULT fresult;
  int len_status;

  if (is_opened(p_file_desc)) {
printk("opened\n");
    fresult = f_read(&(p_file_desc->fil), buf, (UINT)len, &rlen);
    if (fresult == FR_OK) {
      len_status = (int)rlen;
    } else {
printk("fresult %d\n", fresult);
      /* TODO: errno */
      len_status = -1;
    }
  } else {
printk("not opened\n");
    errno = EBADF;
    len_status = -1;
  }

  return len_status;
}

ssize_t
mrb_mruby_io_libnosys_write(int posix_fd, const void *buf, size_t len)
{
  int internal_fd = POSIX_FD_TO_INTERNAL(posix_fd);
  file_desc_t *p_file_desc = &file_desc[internal_fd];
  UINT wlen;
  FRESULT fresult;
  ssize_t len_status;

  if (is_opened(p_file_desc)) {
    FIL *p_fil = &(p_file_desc->fil);
printk("write: opened\n");
    fresult = f_write(p_fil, buf, (UINT)len, &wlen);
    if (fresult == FR_OK) {
      fresult = f_sync(p_fil);
    }

    if (fresult == FR_OK) {
      len_status = (ssize_t)wlen;
    } else {
printk("fresult %d\n", fresult);
      /* TODO: errno */
      len_status = -1;
    }
  } else {
printk("write: not opened\n");
    errno = EBADF;
    len_status = -1;
  }

  return len_status;
}

/******************************/

void
mrb_mruby_io_libnosys_posix_init(mrb_state *mrb)
{
  int i;
  TCHAR letter[] = _TEXT("0:");
  FRESULT fresult;

  for (i = 0; i < MRUBY_IO_LIBNOSYS_DRIVE_MAX; i++) {
    letter[i] = '0' + i;
    fresult = f_mount(&fatfs[i], letter, 1);
    printf("fmount(%d) -> %d", i, fresult);
  }

  init_file_desc();

  for (i = 0; i < MRUBY_IO_LIBNOSYS_FD_MAX; i++) {
    file_desc[i].posix_fd = INTERNAL_TO_POSIX_FD(i);
    push_file_desc(&file_desc[i]);
  }
}

void
mrb_mruby_io_libnosys_posix_final(mrb_state *mrb)
{
  int i;
  TCHAR letter[] = _TEXT("0:");

  /* close all file descriptors to make sure. */
  for (i = 0; i < MRUBY_IO_LIBNOSYS_FD_MAX; i++) {
    f_close(&(file_desc[i].fil));
  }

  /* unmount all drives. */
  for (i = 0; i < MRUBY_IO_LIBNOSYS_DRIVE_MAX; i++) {
    letter[0] = '0' + i;
    f_mount(NULL, letter, 0);
  }
}

