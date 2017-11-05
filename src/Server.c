/*
 * Server.c
 *
 *  Created on: Jul 7, 2017
 *      Author: Pavel Korolin
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "Net.h"
#include "Server.h"
#include "vector.h"

#define FAIL -1
#define SUCCESS 1
#define ALIVE 1 /*server is alive*/
#define DEAD 0 /*server is alive*/
#define NO_ACTION_TAKEN 0 /*for vector for each function*/
#define ACTION_TAKEN 1 /*for vector for each function*/
#define MAGICNUMBER 0xDEADFACE
#define BUFFER_SIZE 1024 /*Maximum buffer size*/

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

struct Servet_t
{
	size_t m_magicNumber;
	size_t m_maxNumOfClients; /*the maximum client allow to connect to server  */
	size_t m_numberOfConnectedClients; /*number of connected clients*/
	Socket m_masterSocket; /*server input socket*/
	size_t m_isAlive; /*to stop the server raning*/
	fd_set m_masterRfds; /*master set to avoid make set from begin all while loop*/
	fd_set m_tempRfds; /*master set to avoid make set from begin all while loop*/
	size_t m_mx_fd; /*maximum file descriptor need for select*/
	AcceptFunction m_acceptfunction; /*user provided function for new socket accept*/
	RecieveDatafunction m_resieveFunction; /*user provided function for received data*/
    DisconectClientfunction m_disconectClientFunc;  /*user provided function for disconected clinets data*/
	void* m_context; /*user context provided in Server_Run function*/
 	Vector_t* m_sockets; /*sockets cotainer*/
};

/*****************static function*********/
/*initialize server conection status bind and itc..*/
static int ServerInitialize(Servet_t* _server, ServerInit* _initInfo);
/*internal function used for free items in the vector*/
static void DestroyItem(void* _socket);
/*vector for each function*/
static int CheckAndResiveFunction(void* _element, size_t _index, void* _context);
/*accept new connections that wait on the waiting list*/
static int AcceptNewConnections(Servet_t* _server);
/*the function check error from accept() and return true if the error happened cause real problem */
static int IsRealError();
/*the function set the socket number in the socket vector as usable*/
static int AddNewSocket(Servet_t* _server, Socket _socketToAdd);
/*the function set the socket number in socket vector as not usable*/
static int RemoveSocket(Servet_t* _server, Socket _socketToRemove);
/*go throw all sockets and check if new data receive the function check only until result of select() function = 0
 * if the function revals socket that close from client side, than RemoveSocket function call
 * */
static int ReciveDataFromSockets(Servet_t* _server, int _result);

/*********************************************/
/********************************************/
Servet_t* Server_Initialize(ServerInit* _initInfo)
{
	Servet_t* newServer;
	int res = 0;
	if(NULL == _initInfo)
		return NULL;
	newServer = malloc(sizeof(Servet_t));
	if(NULL == newServer)
		return NULL;
	res = ServerInitialize(newServer, _initInfo);
	if(FAIL == res)
	{
		free(newServer);
	}

	newServer->m_maxNumOfClients = _initInfo->m_maxNumberOfClient;
	newServer->m_acceptfunction = _initInfo->m_acceptFunction;
	newServer->m_resieveFunction = _initInfo->m_recieveDatafunction;
	newServer->m_disconectClientFunc = _initInfo->m_disconectClientFunc;
	newServer->m_numberOfConnectedClients = 0;
	newServer->m_isAlive = ALIVE;
	newServer->m_magicNumber = MAGICNUMBER;

return newServer;
}

/******************************/
int Server_Close(Servet_t* _serverToclose)
{
	DisconnectSocket(_serverToclose->m_masterSocket);
	Vector_Destroy(&_serverToclose->m_sockets,(DestroyAction)DestroyItem);
	_serverToclose->m_magicNumber = 0;
	free(_serverToclose);

return SUCCESS;
}

/**********************************/
int Server_Run(Servet_t* _serverToRun, void* _context)
{
	struct timeval tv,timeout;
	int result = 0;
	tv.tv_sec =1;
	tv.tv_usec = 0;
	_serverToRun->m_context = _context;
	FD_ZERO(&_serverToRun->m_masterRfds);
	FD_SET(_serverToRun->m_masterSocket, &_serverToRun->m_masterRfds);
	printf("start server \n");
	while(_serverToRun->m_isAlive)
	{
		timeout = tv;
		_serverToRun->m_tempRfds = _serverToRun->m_masterRfds;
		result = select(_serverToRun->m_mx_fd+1 , &_serverToRun->m_tempRfds, NULL , NULL , &timeout);
        if ((0 > result) && (errno!=EINTR))
        {
            return FAIL;
        }
        else if(0 > result)
        {
        	/*if was an error read again*/
        	continue;

        }
        /*check if new connection wait to accept*/
        if(FD_ISSET(_serverToRun->m_masterSocket, &_serverToRun->m_tempRfds))
		{
        	--result;
        	AcceptNewConnections(_serverToRun);
        }

        /*recive data from sockets*/
        if(result)
		{
			ReciveDataFromSockets(_serverToRun, result);
		}

	}

return SUCCESS;
}

/************************************************/
int Server_Send(Socket _socket, void* _data, size_t _dataLength)
{
	return Send(_socket,_data,_dataLength);
}

/******************************************/
void Server_Stop(Servet_t* _serverToStop)
{
	_serverToStop->m_isAlive = DEAD;
}

/**************internal static function ****************************/
static int ReciveDataFromSockets(Servet_t* _server, int _result)
{
	return Vector_ForEeach(_server->m_sockets,(VectorElementAction)CheckAndResiveFunction, _result,(void*)_server);
}

/*********************************/
static int AcceptNewConnections(Servet_t* _server)
{
	sockaddr_in clientSin;
	size_t addrlen = sizeof(sockaddr_in);
	/*accept new client connection */
	Socket newSocket = accept(_server->m_masterSocket, (sockaddr*)&clientSin,(socklen_t*) &addrlen);
	if(0 > newSocket && IsRealError() )
	{
		return FAIL;
	}

	if(_server->m_numberOfConnectedClients == _server->m_maxNumOfClients)
	{
		DisconnectSocket(newSocket);
		return SUCCESS;
	}

	if(!AddNewSocket(_server,newSocket))
	{
		return FAIL;
	}

	_server->m_acceptfunction(newSocket, &clientSin, _server->m_context);

return SUCCESS;
}


/*******************************************************/
static int AddNewSocket(Servet_t* _server, Socket _socketToAdd)
{
	Socket* temp = NULL;
	/*Vector count From 1 */
	if(VECTOR_SUCCESS != Vector_Get(_server->m_sockets,_socketToAdd + 1,(void**)&temp))
	{
		return FAIL;
	}

	*temp = _socketToAdd; /*set the number of the new socket in the socket vector */

	FD_SET(_socketToAdd, &_server->m_masterRfds);

	if(_socketToAdd > _server->m_mx_fd)
		_server->m_mx_fd = _socketToAdd;

	++_server->m_numberOfConnectedClients;

return SUCCESS;
}

/*******************************************************/
static int RemoveSocket(Servet_t* _server, Socket _socketToRemove)
{
	Socket* temp = NULL;
	/*Vector count From 1 */
	if(VECTOR_SUCCESS != Vector_Get(_server->m_sockets,_socketToRemove + 1,(void**)&temp))
	{
		return FAIL;
	}
	*temp = 0;/*set the 0 in the socket vector */

	DisconnectSocket(_socketToRemove);
	_server->m_disconectClientFunc(_socketToRemove,_server->m_context);
	FD_CLR(_socketToRemove, &_server->m_masterRfds);
	--_server->m_numberOfConnectedClients;

return SUCCESS;
}

/******************************/
static int IsRealError()
{
	return errno != EAGAIN && errno != EWOULDBLOCK;
}

/*********************************/
static int ServerInitialize(Servet_t* _server, ServerInit* _initInfo)
{
	sockaddr_in serverData;
	int val = 1;
	int i = 0;
	Socket* temp;
	_server->m_masterSocket = ConnectSocket(_initInfo->m_ip,_initInfo->m_port);
	if(0 > _server->m_masterSocket)
	{
		return FAIL;
	}
	memset(&serverData,0,sizeof(serverData));
	serverData.sin_family = AF_INET;
	serverData.sin_port  = htons(_initInfo->m_port);
	serverData.sin_addr.s_addr 	= INADDR_ANY;

	if (setsockopt(_server->m_masterSocket, SOL_SOCKET, SO_REUSEADDR, &val ,sizeof(int))< 0)
		return FAIL;

	if(bind(_server->m_masterSocket , (struct sockaddr*)(&serverData),sizeof(serverData)) < 0)
	{
		DisconnectSocket(_server->m_masterSocket);
		return FAIL;
	}

	if(listen( _server->m_masterSocket,_initInfo->m_waitingListSize) < 0)
	{
		DisconnectSocket(_server->m_masterSocket);
		return FAIL;
	}


	_server->m_sockets = Vector_Create(_initInfo->m_maxNumberOfClient,_initInfo->m_maxNumberOfClient*2);

	/*create new vector whit max capacity and set all the sockets to 0 */
	for(i = 0 ; i < _initInfo->m_maxNumberOfClient ; ++i )
	{
		temp = malloc(sizeof(int));
		*temp = 0;
		if(VECTOR_SUCCESS != Vector_Append(_server->m_sockets,(void*)temp))
		{
			Vector_Destroy(&_server->m_sockets,(DestroyAction)DestroyItem);
			DisconnectSocket(_server->m_masterSocket);
			return FAIL;
		}
	}
    _server->m_mx_fd = _server->m_masterSocket;

return SUCCESS;
}

/*****************Vector Function************************/
/********************************************************/
static int CheckAndResiveFunction(void* _element, size_t _index, void* _context)
{
	int huwMuchReceive = 0;
	char buffer[BUFFER_SIZE] = {0};
	Socket* temp = (Socket*) _element;
	Servet_t* server = (Servet_t*)_context;

	if(0 != *temp && FD_ISSET(*temp, &server->m_tempRfds))
	{

		do{
			huwMuchReceive = Receive(*temp,buffer,BUFFER_SIZE);
			if(0 > huwMuchReceive)
			{
				if(IsRealError())
				{
					return FAIL;
				}
			}
			if(0 == huwMuchReceive)
			{
				RemoveSocket(server,*temp);
				return NO_ACTION_TAKEN;
			}
		}while(server->m_resieveFunction(*temp, buffer, huwMuchReceive,server->m_context));

    return ACTION_TAKEN;
	}

return NO_ACTION_TAKEN;
}

/********************************************************/
static void DestroyItem(void* _socket)
{
	Socket* temp = (Socket*)_socket;
	if(0 < *temp)
	{
		DisconnectSocket(*temp);
	}
	free(temp);
}
