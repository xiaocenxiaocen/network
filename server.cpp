#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "rio.h"
#include "util.h"
#include "task.h"

typedef struct sockaddr SA;
#ifndef LISTENQ
#define LISTENQ 1024
#endif

int open_listenfd(int port)
{
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	/* Create a socket descriptor */
	if ((listenfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	/* Eliminates "Address already in use" error from bind */
//	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSERADDR, 
//		       (const void *)&optval, sizeof(int)) < 0)
//		return -1;

	/* Listenfd will be an end point for all requests to port
	   on any IP address for this host */
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;

	/* Make it a listening socket ready to accept connection requests */
	if (listen(listenfd, LISTENQ) < 0)
		return -1;
	return listenfd;
}


#define MAXLINE 1024
void echo(int connfd)
{
	size_t n;
	char buf[MAXLINE];
	rio_t rio;

	Rio_readinitb(&rio, connfd);
	while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		fprintf(stdout, "INFO: server received %ld bytes\n", n);
		Rio_written(connfd, buf, n);
	}	
}

int main(int argc, char * argv[])
{
	if(argc != 2) {
		fprintf(stdout, "USAGE: %s <port>\n", argv[0]);
		return -1;
	}
	int listenfd;
	int port = atoi(argv[1]);
	if((listenfd = open_listenfd(port)) < 0) {
		fprintf(stderr, "ERROR: errno returned by open_listenfd() is %d\n", listenfd);
		return -2;
	}
	
	struct sockaddr_in clientaddr;
	struct hostent * hp;
	char * haddrp;
	int connfd, clientlen;
	while(1) {
		clientlen = sizeof(clientaddr);
		connfd = accept(listenfd, (SA *)&clientaddr, (unsigned int *)&clientlen);

		/* Determine the domain name and IP address of the client */
		hp = gethostbyaddr((const char*)&clientaddr.sin_addr.s_addr, 
				   sizeof(clientaddr.sin_addr.s_addr), PF_INET);
		haddrp = inet_ntoa(clientaddr.sin_addr);
		fprintf(stdout, "INFO: server connected to %s (%s)\n", hp->h_name, haddrp);
		fflush(stdout);
		
		echo(connfd);
		close(connfd);	
	}

	return EXIT_SUCCESS;
}
