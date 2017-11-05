/*
 ============================================================================
 Name        : ClientSide.c
 Author      : Pavel Kolrolin
 Version     : 1
 Copyright   : MIT
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "ClientLogic.h"
ClientLogic_t* g_client =NULL;

static void UsrHostSigAbort(int pSignal)
{
	clientLogic_Stop(g_client);
}

static void HandleHostSignal(void)
{
	struct sigaction sa;

    sigemptyset(&sa.sa_mask);
	sa.sa_handler = &UsrHostSigAbort;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

}

int main(int argc , char *argv[])
{
	char ip[16] = {0};
    ClientLogic_t* client;
    size_t port;
    int user;
    HandleHostSignal();

    if(argc == 3)
    {
        port = atoi(argv[2]);
        strcpy(ip,argv[1]);
        client = clientLogic_Create(ip ,port);
    }
    else
    {
        printf("\n please select user to conect: \n");
        printf("1 for T is in the house tomer \n");
        printf("2 for the man whit 100 ids guy \n");
        printf("3 for Manager Manager youval \n");
        printf("4 for Self listing  \n");
        printf("5 for pavel \n");
        user = 4;

        switch(user)
        {
            case 1:
                {
                    client = clientLogic_Create("192.168.0.58" ,7254);
                    break;
                }
            case 2:
                {
                    client = clientLogic_Create("192.168.0.67" ,7984);
                    break;

                }
            case 3:
                {
                    client = clientLogic_Create("192.168.0.48" ,4848);
                    break;
                }
            case 4:
                {
                    client = clientLogic_Create("127.0.0.1" ,7254);
                    break;

                }
            case 5:
                {
                    client = clientLogic_Create("192.168.0.17" ,7254);
                    break;

                }
        }
    }

    if(!client)
    {
            return 0;
    }
    g_client = client;
    clientLogic_Run(client);
    clientLogic_Destroy(client);
	return EXIT_SUCCESS;
}
