/*
 * Net.h
 *
 *  Created on: Jul 7, 2017
 *      Author: Pavel Korolin
 *      this module is the base module for network TCP communication
 */

#ifndef NET_H_
#define NET_H_
#include <stddef.h>/*for size_t*/

typedef int Socket;

/**
 * @brief the function Connected to given ip and port
 * @param const char* _ip - target ip
 * @param size_t _port - target port
 * @return in success Socket >0 on error Socket < 0 and errno set
 */
Socket ConnectSocket(const char* _ip, size_t _port);

/**
 * @brief the function Disconnect a given socket
 * @param int _fd - socket
 * @return in success 1 in fail 0
 */
int DisconnectSocket(Socket _socket);

/**
 * @brief the function Send data to given socket
 * @param Socket _socket - socket to send to her, the socket must be connected before use other wise fail return
 * @param const void* _data - data to sen
 * @param size_t _length - lenght of the data
 * @return in success int >0 on error int < 0 and errno set
 * @warning if _length  == 0 return -1;
 */
int Send(Socket _socket, const void* _data, size_t _length);

/**
 * @brief the function Receive data from given socket
 * @param Socket _socket - socket to receive from, the socket must be connected before use other wise fail return
 * @param void* _buffer - buffer to insert to him a data that receive
 * @param size_t _howMuchToRead - how much to read from the socket
 * @return in success int >0 on error int < 0 and errno set
 * @warning  if return 0 check errno if the conaction wasn't close
 * @warning if _howMuchToRead  == 0 return -1;
 */
int Receive(Socket _socket, void* _buffer, size_t _howMuchToRead);



#endif /* NET_H_ */
