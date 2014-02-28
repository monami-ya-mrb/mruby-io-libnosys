/* Copyright 2013 Monami-ya LLC, Japan.
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#undef errno
extern int errno;

int
_DEFUN (lstat, (path, arg),
	const char *path _AND struct stat *buf)
{
	errno = ENOSYS;
	return -1;
}

