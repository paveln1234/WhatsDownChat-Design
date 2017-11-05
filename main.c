/*
 ============================================================================
 Name        : TcpChatProject.c
 Author      : Pavel Kolrolin
 Version     :
 Copyright   : MIT
 Description : Server Logic part
 ============================================================================
 */

#include <signal.h>
#include <sys/time.h>

#include "ServerLogic.h"

ServerLogic_t* g_server= NULL;

static void UsrHostSigAbort(int pSignal)
{
	ServerLogic_Stop(g_server);
}

static void HandleHostSignal(void)
{
    struct sigaction satmp;
    sigemptyset(&satmp.sa_mask);
    satmp.sa_flags = 0;
    satmp.sa_handler = UsrHostSigAbort;
    sigaction(SIGINT, &satmp, NULL);
}

/********************************************/
int main(void)
{
    ServerLogic_t* server = NULL;

    HandleHostSignal();

    server = ServerLogic_Create(150,150);
	g_server = server;

    ServerLogic_Run(server);
    ServerLogic_Destroy(server);

return EXIT_SUCCESS;
}
