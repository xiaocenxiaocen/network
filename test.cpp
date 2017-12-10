#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

static char * get_time(time_t t)
{
	static char sbuf[20];
	char * now = ctime(&t);
	int len;
	time_t at = t;

	if(at == 0) at = time(NULL);
	now = ctime(&at);
	len = strlen(now);
	if(len < 19) {
		fprintf(stderr, "ERROR: error in ctime: %s\n", now);
	}
	memcpy(sbuf, now, 19);
	sbuf[19] = '\0';
	return sbuf + 4;	
}

int main(int argc, char * argv)
{
	
	time_t t;
	time(&t);
	char * ltime = get_time(t);
	
	char start_time[20];
	memcpy(start_time, ltime, strlen(ltime));
	fprintf(stdout, "%s, %d\n", start_time, strlen(start_time));
	

	return EXIT_SUCCESS;
}
