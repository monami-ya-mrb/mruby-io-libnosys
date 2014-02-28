/* Copyright 2013 Monami-ya LLC, Japan.
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <sys/stat.h> 
#include <sys/types.h> 
#include <errno.h>
#include <unistd.h>
#undef errno
extern int errno;

int
_DEFUN (execl, (path, buf, ...),
	const char *path _AND const char *arg _DOTS)
{
	errno = ENOSYS;
	return -1;
}

