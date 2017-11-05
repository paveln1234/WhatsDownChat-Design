#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT "\x1B[37m"
#define FIFO_PATH "./msgfifo"
#define HELLO_PORT 12345
#define HELLO_GROUP "225.0.0.37"

int main(int argc, char *argv[])
{
	struct sockaddr_in addr;
	int fd, cnt;
	struct ip_mreq mreq;
	char message[1024]="Hello, World!";
	int port = atoi(argv[2]);
	char ip;
	int howMuschRead;
    pid_t pid;
	int fifo = 0;

	/*if open faild nothing writer to do*/
	if(0>(fifo = open(FIFO_PATH, O_WRONLY)))
	{
		perror("fifo open");
		return 1;
	}
    pid = getpid();
    write(fifo, (void*)&pid, sizeof(pid));

     /* create what looks like an ordinary UDP socket */
     if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
	  perror("socket");
	  exit(1);
     }
     /* set up destination address */
     memset(&addr,0,sizeof(addr));
     addr.sin_family=AF_INET;
     addr.sin_addr.s_addr=inet_addr(argv[1]);
     addr.sin_port=htons(port);

     /* now just sendto() our destination! */
	printf("writer \n");
     while (1)
     {
			howMuschRead = sprintf(message,KRED "%s%s" KNRM, argv[3], ": ");
			fgets(message + howMuschRead, 1024 - howMuschRead, stdin);
			message[strcspn(message, "\n")] = 0;
			if (sendto(fd,message,sizeof(message),0,(struct sockaddr *) &addr, sizeof(addr)) < 0)
			{
			   perror("sendto");
			   exit(1);
			}
     }
     close(fifo);
return 0;
}
