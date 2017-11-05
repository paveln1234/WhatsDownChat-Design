/****************************************************
 *  Protocol.h
 *  Created on: 12-7-2017 10:57:48
 *  Implementation of the Class Protocol
 *  Original author: A-Team
 ****************************************************/

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>


#define BUFFER_SIZE 1024

#define TLV_TYPE_LENGTH 1
#define TLV_LENGTH_LENGHT 5
#define HEADER_LENGTH 6
#define MAX_MESSAGE_LENGTH 1024
#define MAX_USERNAME 32
#define MAX_PASSWORD 32
#define MAX_GROUP_NAME 32
#define MAX_IP_LENGTH 16
#define MAX_PORT_LENGHT 5
#define ourInt 32
#define DELIMITER "@@@"
#define DELIMITER_LENGHT 3


/**
 * The Module is apart of Chat app project.
 * Protocol module Responsible to Encode and Decode Messages between
 * server client
 */

typedef enum bool
{
    TURE,
    FALSE
}bool;


typedef struct sockaddr_in sockaddr_in_t;
/*
 * struct sockaddr_in {
 *     short            sin_family;   // e.g. AF_INET
 *     unsigned short   sin_port;     // e.g. htons(3490)
 *     struct in_addr   sin_addr;     // see struct in_addr, below
 *     char             sin_zero[8];  // zero this if you want to
 * };
 */


/* define Message types between client server as enum  */
typedef enum MessageType
{
	MESSAGETYPE_RESPONS = 65,
	MESSAGETYPE_SIGNUP ,
	MESSAGETYPE_LOGIN,
	MESSAGETYPE_LOGOUT,
	MESSAGETYPE_DELETE_USER,
	MESSAGETYPE_JOIN_GROUP,
	MESSAGETYPE_CREATE_GROUP,
	MESSAGETYPE_LEAVE_GROUP,
	MESSAGETYPE_GET_ALL_GROUPS,
}MessageType;


typedef enum BackEndStatus
{
	BackEnd_SUCCESS = 66,
	BackEnd_SYSTEM_FAIL = 67,
	BackEnd_UNKNOWN_ERROR= 68,
	BackEnd_USER_NAME_TAKEN = 69,
	BackEnd_USER_NAME_INVALID = 70,
	BackEnd_USER_NOT_FOUND = 71 ,
	BackEnd_PASSWORD_INVALID = 72,
	BackEnd_GROUP_NAME_TAKEN = 73,
	BackEnd_GROUP_NAME_INVALID,
	BackEnd_GROUP_DO_NOT_EXIST,
	BackEnd_MORE_INFO_INCOMING,
	BackEnd_NO_FREE_IP,
	BackEnd_CLIENT_DISCONNECTED,
	BackEnd_REQUST_UNKNOWN
} BackEndStatus ;


typedef struct ServerReceiveMessage
{
    MessageType m_MessageType;
    char m_userName[MAX_USERNAME];
    char m_password[MAX_PASSWORD];
    char m_groupName[MAX_GROUP_NAME];
} ServerReceiveMessage_t ;

typedef struct ClientReceiveMessage
{
    MessageType m_messageType;
    BackEndStatus m_status;
    char m_ip[MAX_IP_LENGTH]; /*use for ip@@@port@@@ or for grups whit@@@ betwin them*/
    char port[MAX_PORT_LENGHT]; /*0 - 65000*/
    char m_data[MAX_MESSAGE_LENGTH]; /*use for ip@@@port@@@ or for grups whit@@@ betwin them*/
    size_t m_numberOfGroups;
} ClientReceiveMessage_t;



/* ~~~ Request functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _groupName - group name
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeNewGroup(const char* _groupName, void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeGetAllGroups(void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _groupNameToJoin - group name to join
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeJoinGroup(const char* _groupNameToJoin, void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _groupNameToLeave - group name to Leave
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLeaveGroup(const char* _groupNameToLeave, void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _userName - User Name
  * @param Char* _password - Password
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLogIn(const char* _userName, const char* _password, void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLogOut(void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _newUserName - User Name
  * @param Char* _password - Password
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeSignUp(const char* _newUserName, const char* _password, void* _buffer);


 /**
  * @brief The function Encode Message to Protocol format
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeDeleteUser(void* _buffer);

 /**
  * @brief The function Decode Message from  Protocol format to ServerReceiveMessage_t form
  * @param void* _dataToDecode - pointer to data that need to decoded
  * @param size_t _length - Data length
  * @param ServerReceiveMessage_t* _message - Receive Message that the function fill
  * @return on success true,  on fail false.
  */
bool Protocol_DecodeServer(void* _dataToDecode, size_t _lenght, ServerReceiveMessage_t* _message );
/**
 * @brief The function Decode Message from  Protocol format to ClientReceiveMessage_t form
 * @param void* _dataToDecode - pointer to data that need to decoded
 * @param size_t _length - Data length
 * @param ClientReceiveMessage_t* _message - Receive Message that the function fill
* @return on success true,  on fail false.
 */
bool Protocol_DecodeClient(void* _dataToDecode, size_t _lenght, ClientReceiveMessage_t* _message );


/* ~~~ Response functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/**
 * @brief encode the response from server-end to front-end of sign-up respond
 * @param _responseStatus ENUM status of back-end result
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeSignUp_Response(BackEndStatus _responseStatus, void* _buffer);
/**
 * @brief encode the response from server-end to front-end of delete user respond
 * @param _responseStatus ENUM status of back-end result
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeDeleteUser_Response(BackEndStatus _responseStatus, void* _buffer);

/**
 * @brief encode the response from server-end to front-end of log-in respond
 * @param _responseStatus ENUM status of back-end result
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeLogIn_Response(BackEndStatus _responseStatus, void* _buffer);

/**
 * @brief encode the response from server-end to front-end of log-out respond
 * @param _responseStatus ENUM status of back-end result
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeLogOut_Response(BackEndStatus _responseStatus, void* _buffer);

/**
 * @brief encode the response from server-end to front-end of create new group respond
 * @param _responseStatus ENUM status of back-end result
 * @param m_groupAdrres a struct that continues ip and port
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeNewGroup_Response(BackEndStatus _responseStatus, sockaddr_in_t m_groupAdrres, void* _buffer);

/**
 * @brief encode the response from server-end to front-end of join group respond
 * @param _responseStatus ENUM status of back-end result
 * @param m_groupAdrres a struct that continues ip and port
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeJoinGroup_Response(BackEndStatus _responseStatus, sockaddr_in_t m_groupAdrres, void* _buffer);

/**
 * @brief encode the response from server-end to front-end of leave group respond
 * @param _responseStatus ENUM status of back-end result
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeLeaveGroup_Response(BackEndStatus _responseStatus, void* _buffer);


/**
 * @brief encode the response from server-end to front-end of show all group respond
 * @param _responseStatus ENUM status of back-end response
 * @param _groupsNames a string of groups names devided by \0
 * @param _numGroups the numer of groups in _groupsNames
 * @param _buffer the buffer in which msg to be send
 * @return on success message length. on fail -1
 */
int Protocol_EncodeGetAllGroups_Response(BackEndStatus _responseStatus, const char* _groupsNames, size_t _numGroups, void* _buffer);


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"


#define TLV_WRITE( BUFFER ,FORMAT, ...) \
    Tlv_Decode((BUFFER),(FORMAT),__VA_ARGS__)

#pragma GCC diagnostic pop


#endif /*_PROTOCOL_H*/

