#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rio.h"
#include "util.h"
#include "task.h"

typedef struct sockaddr SA;

int open_clientfd(char * hostname, int port)
{
	int clientfd;
	struct hostent * hp;
	struct sockaddr_in serveraddr;
	
	fprintf(stdout, "INFO: Create a socket\n");
	if ((clientfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		return -1; /* Check errno for cause of error */
	}
		
	/* Fill in the server's IP address and port */
	if ((hp = gethostbyname(hostname)) == NULL) {
		return -2; /* Check h_errno for cause of error */
	}
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = PF_INET;
	bcopy((char *)hp->h_addr_list[0], 
	      (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
	serveraddr.sin_port = htons(port);

	fprintf(stdout, "INFO: Connect to server\n");	
	/* Establish a connection with the server */
	if (connect(clientfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0) {
		return -1;
	}	

	return clientfd;	
}

#define MAXLINE 1024

int main(int argc, char * argv[])
{
	if(argc != 3) {
		fprintf(stdout, "USAGE: %s <host> <port>\n", argv[0]);
		return -1;
	}
	char * host = argv[1];
	int port = atoi(argv[2]);
	int clientfd;
	if((clientfd = open_clientfd(host, port)) < 0) {
		fprintf(stderr, "ERROR: errno returned by open_clientfd() is %d\n", clientfd);
		return -2;
	}
	fprintf(stdout, "INFO: successfully connect to host %s\n", argv[1]);
	
	char buf[MAXLINE];
	rio_t rio;
	Rio_readinitb(&rio, clientfd);

	while( fgets(buf, MAXLINE, stdin) != NULL) {
		Rio_written(clientfd, buf, strlen(buf));
		Rio_readlineb(&rio, buf, MAXLINE);
		fputs(buf, stdout);
	}
	close(clientfd);
	
	return EXIT_SUCCESS;
}
