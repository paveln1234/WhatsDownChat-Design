/*
 * Client.h
 *
 *  Created on: Jul 13, 2017
 *      Author: pavel
 */

#ifndef CLIENT_H_
#define CLIENT_H_


#define MESSAGE_LENGTH 1024
#define USERNAME_LENGTH 32
#define PASSWORD_LENGTH 32
#define GROUP_NAME_LENGTH 32


typedef int Socket;
typedef struct Client_t Client_t;


typedef struct ClientInit
{
	char* m_ip;
	size_t m_port;

}ClientInit;


/**
 * @brief the function initial the client
 * @param const char* _serverIp - server ip
 * @param size_t port - server port
 * @return if success Client_t pointer in fail NULL
 */
Client_t* Client_Create(const char* _serverIp, size_t _port);

/**
 * @brief the function close client
 * @param Client_t* _clientToclose - client to close
 * @return in success 1 in fail -1
 */
int Client_Destroy(Client_t* _clientToclose);


/**
 * @brief The function Send data from agiven socket
 * @param Client_t _client - client to send from hem
 * @param void* _data - data to send
 * @param size_t _dataLength - data lenght to send
 * @return in success int > 0 number of bytes that send in fail int < 0 and user should check errno
 */
int Client_Send(Client_t* _client,void* _data,size_t _datasize);

/**
 * @brief The function resive data from server
 * @param Client_t _client - client to send from hem
 * @param void* _buffer - buffer for received data
 * @return in success int > 0 number of bytes that send in fail int < 0 and user should check errno
 * @warning if data == 0  server fail
 */
int Client_Resive(Client_t* _client,void* _buffer);



#endif /* CLIENT_H_ */
