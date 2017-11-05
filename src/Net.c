/*
 * Net.c
 *
 *  Created on: Jul 7, 2017
 *      Author: pavel
 */
#include <errno.h>
#include <unistd.h>   /*close*/
#include <arpa/inet.h>    /*close*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "Net.h"

/**********************************/
Socket ConnectSocket(const char* _ip, size_t _port)
{
	Socket newSocket = 0;
	if(NULL == _ip)
		return newSocket;
	newSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > newSocket)
		return newSocket;
return newSocket;
}

/****************************************************/
int DisconnectSocket(Socket _socket)
{

	int status = 0;
	status = close(_socket);
	if(0>status)
		return -1;
return status;
}

/**********************************/

int Send(Socket _socket, const void* _data, size_t _length)
{
	int howMuchSend = -1;
	if(NULL == _data || 0 == _length)
	{
		return howMuchSend;
	}

	howMuchSend = send(_socket,_data,_length,0);

return howMuchSend;
}

int Receive(Socket _socket, void* _buffer, size_t _howMuchToRead)
{
	int howMuchReceive = -1;
	if(NULL == _buffer || 0 == _howMuchToRead)
	{
		return howMuchReceive;
	}
	howMuchReceive = recv(_socket,_buffer,_howMuchToRead,0);

return howMuchReceive;
}


