/*
 * Server.h
 *
 *  Created on: Jul 7, 2017
 *      Author: Pavel Korolin
 */

#ifndef SERVER_H_
#define SERVER_H_

typedef int Socket;
typedef struct Servet_t Servet_t;

 /**
  * @brief the function return socket number and socket information
  * @param Socket _socket - socket number
  * @param struct sockaddr_in* _inCommigSocketInfo - Socket information
  * @param void* _context - user context provided in Server_Run function
  * @return in success 1 in fail -1
  */
typedef int(*AcceptFunction)(Socket _socket,struct sockaddr_in* _inCommigSocketInfo, void* _context);

/**
 * @brief the function return data that receive from the user
 * @param Socket _socket - socket number
 * @param void* _data- data that receive
 * @param size_t _datalength - data length
 * @param void* _context - user context provided in Server_Run function
 * @return	if more receive needed return 1, if no more data receive needed  return 0;
 */
typedef int(*RecieveDatafunction)(Socket _socket,void* _data, size_t _datalength,void* _context);

/**
 * @brief the function for disconected clint
 * @param Socket _socket - disconect socket number
 * @param void* _context - user context provided in Server_Run function
 * @return	if more receive needed return 1, if no more data receive needed  return 0;
 */
typedef int(*DisconectClientfunction)(Socket _socket,void* _context);

/*define Struct server init that nead for config the serve*/
typedef struct ServerInit
{
	char* m_ip;
	size_t m_port;
	size_t m_maxNumberOfClient;
	size_t m_waitingListSize;
	AcceptFunction m_acceptFunction;
	RecieveDatafunction m_recieveDatafunction;
	DisconectClientfunction m_disconectClientFunc;
}ServerInit;

/**
 * @brief the function initial the server
 * @param ServerInit* _initInfo - the information that need to initialize the Server
 * @return if success Servet_t pointer in fail NULL
 */
Servet_t* Server_Initialize(ServerInit* _initInfo);

/**
 * @brief the function close server
 * @param Servet_t* _serverToclose - server to close
 * @return in success 1 in fail -1
 */
int Server_Close(Servet_t* _serverToclose);

/**
 * @brief The function Run Server
 * @param Servet_t* _serverToRun -  server to run
 * @return in success 1 in fail -1 and errno should be checked by the user
 */
int Server_Run(Servet_t* _serverToRun, void* _context);

/**
 * @brief The function Send data from agiven socket
 * @param Socket _socket - socket to send from her
 * @param void* _data - data to send
 * @param size_t _dataLength - data lenght to send
 * @return in success int > 0 number of bytes that send in fail int < 0 and user should check errno
 */
int Server_Send(Socket _socket, void* _data, size_t _dataLength);

/**
 * @brief The function stop server
 * @param _serverToStop
 * @warning the server will complete current task before stop
 */
void Server_Stop(Servet_t* _serverToStop);


#endif /* SERVER_H_ */
