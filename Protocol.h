/****************************************************
 *  Protocol.h
 *  Created on: 12-7-2017 10:57:48
 *  Implementation of the Class Protocol
 *  Original author: A-Team
 ****************************************************/

#ifndef _PROTOCOL_H
#define _PROTOCOL_H


/**
 * The Module is apart of Chat app project.
 * Protocol module Responsible to Encode and Decode Messages between
 * server client
 */


/**
 typedef of Message Types
*/
typedef enum MessageType
{

	SIGNUP, /**/
	LOGIN,
	CREATE_GROUP,
	ENTER_GROUP,
	GET_GROUP_NAMES,
	GET_ALL_GROUPS,
	DISCONNECT,
	LEAVE_GROUP,
	DELETE_USER

}MessageType;

#define BUFFER_SIZE 1024

#define TLV_TYPE_LENGTH 2
#define TLV_LENGTH_LENGHT 5
#define HEADER_LENGTH 7
#define MAX_MESSAGE_LENGTH 1024
#define MAX_USERNAME 32
#define MAX_PASSWORD 32
#define MAX_GROUP_NAME 32
#define NAX_IP_LENGTH 16


#define ourInt 32




/* TLV_HEADER TYPE */
#define TLV_SIGNUP SU
#define TLV_LOGIN LI
#define TLV_LOGOUT LO
#define TLV_DELETE_USER DU
#define TLV_JOIN_GROUP CG
#define TLV_ENTER_GROUP JG
#define TLV_LEAVE_GROUP LG
#define TLV_GET_ALL_GROUPS AG


typedef struct ServerReceiveMessage
{
    MessageType m_MessageType;
    char[MAX_USERNAME] m_userName;
    char[MAX_PASSWORD] m_password;
    char[MAX_GROUP_NAME] m_groupName;
}ReceiveMessage;

typedef struct ClientReceiveMessage
{
    MessageType m_messageType;
    ResultType m_status;
    struct sockaddr_in m_groupAdrres;
    char[MAX_MESSAGE_LENGTH] m_GroupName;
    size_t m_numberOfGroups;
}ReceiveMessage;


 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _groupName - group name
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protoco_EncodeNewGroup(char* _groupName, void* _buffer);

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
int Protocol_EncodeJoinGroup(char* _groupNameToJoin, void* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _groupNameToLeave - group name to Leave
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLeaveGroup(char* _groupNameToLeave, void* _buffer);


 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _userName - User Name
  * @param Char* _password - Password
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLogIn(char* _userName, char* _password, void* buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeLogOut(char* _buffer);

 /**
  * @brief The function Encode Message to Protocol format
  * @param Char* _newUserName - User Name
  * @param Char* _password - Password
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeSignUp(char* _newUserName, char* _password, void* _buffer);


 /**
  * @brief The function Encode Message to Protocol format
  * @param void* _buffer - Buffer that in which the message will written
  * @return on success message length. on fail -1
  */
int Protocol_EncodeDeleteUser(void* _buffer);

 /**
  * @brief The function Decode Message from  Protocol format to RecieveMessage form
  * @param void* _dataToDecode - pointer to data that need to decoded
  * @param size_t _length - Data length
  * @param ReceiveMessage* _message - Receive Message that the function fill
  * @return on success message length. on fail -1
  */
bool Protocol_Decode(void* _dataToDecode, size_t _lenght, ReceiveMessage* _message );



#endif /*_PROTOCOL_H*/
