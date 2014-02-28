/* Copyright 2013 Monami-ya LLC, Japan.
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <sys/file.h>
#include <errno.h>
#undef errno
extern int errno;

int
_DEFUN (flock, (fd, operation),
	int fd _AND int operation)
{
	errno = ENOSYS;
	return -1;
}

