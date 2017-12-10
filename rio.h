#ifndef __RIO_H__
#define __RIO_H__

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define RIO_BUFSIZE 4096
typedef struct {
	int rio_fd;
	int rio_cnt;
	char * rio_bufptr;
	char rio_buf[RIO_BUFSIZE];
}rio_t;

void Rio_readinitb(rio_t * rp, int fd);

static ssize_t Rio_read(rio_t * rp, char * usrbuf, size_t n);

ssize_t Rio_readnb(rio_t * rp, void * usrbuf, size_t n);

ssize_t Rio_readlineb(rio_t * rp, void * usrbuf, size_t maxlen);

ssize_t Rio_written(int fd, void * usrbuf, size_t n);

#endif
