#include "rio.h"

void Rio_readinitb(rio_t * rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
	return;
}

static ssize_t Rio_read(rio_t * rp, char * usrbuf, size_t n)
{
	int cnt;

	while(rp->rio_cnt <= 0) {
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
		if(rp->rio_cnt < 0) {
			if(errno != EINTR) {
				return -1;
			}
		}
		else if(rp->rio_cnt == 0) {
			return 0;
		}
		else {
			rp->rio_bufptr = rp->rio_buf;
		}	
	}

	cnt = n;
	if((size_t)rp->rio_cnt < n) {
		cnt = rp->rio_cnt;
	}
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	
	return cnt;
}

ssize_t Rio_readnb(rio_t * rp, void * usrbuf, size_t n)
{
	size_t leftcnt = n;
	ssize_t nread;
	char * buf = (char *)usrbuf;

	while(leftcnt > 0) {
		if((nread = Rio_read(rp, buf, n)) < 0) {
			if(errno == EINTR) {
				nread = 0;
			} else {
				return -1;
			}
		}
		leftcnt -= nread;
		buf += nread;
	}
	
	return n - leftcnt;
}

ssize_t Rio_readlineb(rio_t * rp, void * usrbuf, size_t maxlen)
{
	size_t n;
	int rd;
	char c, *bufp = (char *)usrbuf;

	for(n = 1; n < maxlen; n++) {
		if((rd = Rio_read(rp, &c, 1)) == 1) {
			*bufp++ = c;
			if(c == '\n') break;
		} 
		else if(rd == 0) {
			if(n == 1) {
				return 0;
			} else {
				break;
			}
		} 
		else {
			return -1;
		}
	}
	*bufp = 0;
	
	return n; 
}

ssize_t Rio_written(int fd, void * usrbuf, size_t n)
{
	size_t nleft = n;
	ssize_t nwritten;
	char * bufp = (char*)usrbuf;

	while(nleft > 0) {
		if((nwritten = write(fd, bufp, nleft)) <= 0) {
			if(errno == EINTR) {
				nwritten = 0;
			} else {
				return -1;
			}
		}
		bufp  += nwritten;
		nleft -= nwritten;
	}
	
	return n;
}
