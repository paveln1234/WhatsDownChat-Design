#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#define MSG_BUF_SIZE 1024
#define FIFO_PATH "./msgfifo"

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, nbytes,addrlen;
	struct ip_mreq mreq;
	char msgbuf[MSG_BUF_SIZE];
	int port;
	char ip[16];
	char groupName[32];
	u_int yes=1;
	pid_t pid;
	int fifo = 0;
	/*if open faild nothing reale to do*/
	if(0> (fifo = open(FIFO_PATH, O_WRONLY)))
	{
		perror("fifo open");
/*		return 1;*/
	}
    pid = getpid();
    write(fifo, (void*)&pid, sizeof(pid));

	if (argc == 4)
	{
		strcpy(ip, argv[1] );
		port = atoi(argv[2]);
		strcpy(groupName,argv[3]);
	}
	else
	{
		strcpy(ip, "225.225.225.225" );
		port = 2255;
		strcpy(groupName, "unknown");
	}

	/* create what looks like an ordinary UDP socket */
	if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
		perror("socket");
/*		return 1;*/
	}

	/* allow multiple sockets to use the same PORT number */
	if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0)
	{
		perror("Reusing ADDR failed");
/*		return 1;*/
	}

	/* set up destination address */
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=inet_addr(ip); /* N.B.: differs from sender */
	addr.sin_port=htons(port);

	/* bind to receive address */
	if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0)
	{
		perror("bind");
		return(1);
	}

	/* use setsockopt() to request that the kernel join a multicast group */

	mreq.imr_multiaddr.s_addr=inet_addr(ip);
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);
	if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0)
	{
		perror("setsockopt failed");
		return(3);
	}

	/* now just enter a read-print loop */
	printf("Reader group:%s (%s)\n--------------\n", groupName, ip);
	while (1)
	{
		addrlen=sizeof(addr);
		if ((nbytes=recvfrom(fd,msgbuf,MSG_BUF_SIZE,0,
				(struct sockaddr *) &addr,&addrlen)) < 0)
		{
			perror("recvfrom");
			return(2);
		}
		printf("***************\n");
		puts(msgbuf);
		printf("***************\n");

	}
	close(fifo);

return 0;
}

