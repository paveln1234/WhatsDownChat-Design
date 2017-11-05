#include <stdio.h>/*sprintf*/
#include <stdlib.h> /*calloc */
#include <sys/types.h>
#include <arpa/inet.h>/*adrr_ntop*/
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include "Protocol.h"
#include "Tlv.h"

static void InitRequst(MessageType _type, char* _str,int _lenght, char* _number);

/********************************************************/
bool Protocol_DecodeServer(void* _dataToDecode, size_t _lenght, ServerReceiveMessage_t* _message )
{
    char* recMsg =(char*)_dataToDecode;
    char* p;
    int temp;
   /* strncpy((char*)temp,recMsg+1,5);*/

    if(NULL == _dataToDecode || NULL == _message)
    {
        return FALSE;
    }

    _message->m_MessageType = recMsg[0] ;

    switch(_message->m_MessageType)
    {
        case MESSAGETYPE_SIGNUP:
        case MESSAGETYPE_LOGIN:
                                {
                                    p =  strtok(recMsg+HEADER_LENGTH,DELIMITER);
                                    strcpy(  _message->m_userName ,p);
                                    p =  strtok(NULL,DELIMITER);
                                    strcpy(  _message->m_password ,p);
                                    break;
                                }
        case MESSAGETYPE_JOIN_GROUP:
                                {
                                temp= atoi(recMsg+1);
                                p =  strtok(recMsg+HEADER_LENGTH,DELIMITER);
                                strncpy(  _message->m_groupName ,p,temp);
                                break;
                                }
        case MESSAGETYPE_CREATE_GROUP:
                            	{
		                            temp= atoi(recMsg+1);
		                            p =  strtok(recMsg+HEADER_LENGTH,DELIMITER);
		                            strncpy(  _message->m_groupName ,p,temp);
		                            break;
                           		 }
        case MESSAGETYPE_LEAVE_GROUP:
                            {
                                p =  strtok(recMsg+HEADER_LENGTH,DELIMITER);
                                strcpy(  _message->m_groupName ,p);
                            }
        default:
                    {
                        break;
                    }
    }

return TURE;
}

bool Protocol_DecodeClient(void* _dataToDecode, size_t _lenght, ClientReceiveMessage_t* _message )
{
    char* recMsg =(char*)_dataToDecode;
    char* p;
    /*int temp = 0;*/
    _message->m_messageType = recMsg[0] ;
     switch(_message->m_messageType)
    {
        case MESSAGETYPE_SIGNUP:
                                {
                                    _message->m_status = recMsg[HEADER_LENGTH];
                                break;
                                }
        case MESSAGETYPE_LOGIN:
                            {
                                _message->m_status = recMsg[HEADER_LENGTH];
                                break;
                            }
        case MESSAGETYPE_JOIN_GROUP:

        case MESSAGETYPE_CREATE_GROUP:
                            {
                                _message->m_status = recMsg[HEADER_LENGTH];
                                p =  strtok(recMsg+HEADER_LENGTH+1,DELIMITER);
                                strcpy(  _message->m_ip ,p);
                                p =  strtok(NULL,DELIMITER);
                                strcpy(  _message->port ,p);
                                break;
                            }
        case MESSAGETYPE_LEAVE_GROUP:
                            {
                                _message->m_status = recMsg[HEADER_LENGTH];
                                break;
                            }
        case MESSAGETYPE_GET_ALL_GROUPS:
                            {
                                _message->m_status = recMsg[HEADER_LENGTH];
                                recMsg = strstr(recMsg+HEADER_LENGTH+DELIMITER_LENGHT,DELIMITER);
                                recMsg = strstr( recMsg,DELIMITER);
                                p =recMsg;
                                   memset(p-DELIMITER_LENGHT,' ',DELIMITER_LENGHT);
                                    *(p-1) = '\n';
                                while(NULL != (p =  strstr(p,DELIMITER)))
                                {
                                    p+=DELIMITER_LENGHT;
                                   memset(p-DELIMITER_LENGHT,' ',DELIMITER_LENGHT);
                                    *(p-1) = '\n';
                                }
                                strcpy(_message->m_data,recMsg);
                                break;
                            }
        default:
                    {
                        break;
                    }
    }
return 1;
}

/********************************************************/
int Protocol_EncodeNewGroup(const char* _groupName, void* _buffer)
{
    int size = 0;
    char c = MESSAGETYPE_CREATE_GROUP;
    char*  number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    /*calculate the length of the data*/
    int length =  strlen(_groupName) + DELIMITER_LENGHT ;
            sprintf(number,"%.5d",length);
    size =  Tlv_Decode(_buffer,"%c%s%s%s",c,number,_groupName,DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeGetAllGroups(void* _buffer)
{
    int size = 0;
    char c = MESSAGETYPE_GET_ALL_GROUPS;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT ;
    sprintf(number,"%.5d",length);
    size =  Tlv_Decode(_buffer,"%c%s%s",c,number,DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeJoinGroup(const char* _groupNameToJoin, void* _buffer)
{
    int size = 0;
    char c = MESSAGETYPE_JOIN_GROUP;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    /*calculate the length of the data*/
    int length =  strlen(_groupNameToJoin) + DELIMITER_LENGHT ;

        sprintf(number,"%.5d",length);

    size =  Tlv_Decode(_buffer,"%c%s%s%s",c,number,_groupNameToJoin,DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeLeaveGroup(const char* _groupNameToLeave, void* _buffer)
{
    int size = 0;
    char c;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    /*calculate the length of the data*/
    int length =  strlen(_groupNameToLeave) + DELIMITER_LENGHT ;

    InitRequst(MESSAGETYPE_LEAVE_GROUP,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%s%s",c,number,_groupNameToLeave,DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeSignUp(const char* _newUserName, const char* _password, void* _buffer)
{
    int size = 0;
    char c = MESSAGETYPE_SIGNUP;
    char* number = calloc(TLV_LENGTH_LENGHT+1, sizeof(char));
    /*calculate the length of the data*/
    int length =  strlen(_newUserName) + DELIMITER_LENGHT + strlen(_password)+DELIMITER_LENGHT;
    sprintf(number,"%.5d",length);
    size =  Tlv_Decode(_buffer,"%c%s%s%s%s%s",c,number,_newUserName,DELIMITER,_password, DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeLogIn(const char* _userName, const char* _password, void* _buffer)
{
    int size = 0;
    char c;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    /*calculate the length of the data*/
    int length =  strlen(_userName) + DELIMITER_LENGHT + strlen(_password)+DELIMITER_LENGHT;
    InitRequst(MESSAGETYPE_LOGIN,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%s%s%s%s",c,number,_userName,DELIMITER,_password, DELIMITER);

    free(number);

return size;
}

/*****************************************************/
int Protocol_EncodeLogOut(void* _buffer)
{

    int size = 0;
    char c;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT ;

    InitRequst(MESSAGETYPE_LOGOUT,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%s",c,number,DELIMITER);

    free(number);


return size;
}

/****************************************************/
int Protocol_EncodeDeleteUser(void* _buffer)
{
    int size = 0;
    char c;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT ;

    InitRequst(MESSAGETYPE_DELETE_USER,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%s",c,number,DELIMITER);

    free(number);

return size;
}

/**********************respons**************************/
int Protocol_EncodeSignUp_Response(BackEndStatus _responseStatus, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT + 1 ;
    InitRequst(MESSAGETYPE_SIGNUP,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s",c,number,resStatus,DELIMITER);

    free(number);

return size;
}

/************************************************/
int Protocol_EncodeDeleteUser_Response(BackEndStatus _responseStatus, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT + 1 ;
    InitRequst(MESSAGETYPE_DELETE_USER,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s",c,number,resStatus,DELIMITER);

    free(number);

return size;
}

/*******************************************************/
int Protocol_EncodeLogIn_Response(BackEndStatus _responseStatus, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT + 1 ;
    InitRequst(MESSAGETYPE_LOGIN,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s",c,number,resStatus,DELIMITER);

    free(number);

return size;
}

/*******************************************************/
int Protocol_EncodeLogOut_Response(BackEndStatus _responseStatus, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT + 1 ;
    InitRequst(MESSAGETYPE_LOGOUT,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s",c,number,resStatus,DELIMITER);

    free(number);

return size;
}

/******************************************************/
int Protocol_EncodeNewGroup_Response(BackEndStatus _responseStatus, sockaddr_in_t m_groupAdrres, void* _buffer)
{
    int size = 0;
    char ipAddress[INET_ADDRSTRLEN];
    size_t port = 0;
    char c;
    char resStatus = _responseStatus;

   	inet_ntop(AF_INET, &(m_groupAdrres.sin_addr), ipAddress, INET_ADDRSTRLEN);
	port = ntohs(m_groupAdrres.sin_port);

    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   1 + DELIMITER_LENGHT+ strlen(ipAddress) +DELIMITER_LENGHT + MAX_PORT_LENGHT+DELIMITER_LENGHT  ;
    InitRequst(MESSAGETYPE_CREATE_GROUP,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s%s%s%0.5d%s",c,number,resStatus,DELIMITER,ipAddress,DELIMITER,port,DELIMITER);

    free(number);

return size;
}

/******************************************************/
int Protocol_EncodeJoinGroup_Response(BackEndStatus _responseStatus, sockaddr_in_t m_groupAdrres, void* _buffer)
{
    int size = 0;
    char ipAddress[INET_ADDRSTRLEN];
    size_t port = 0;
    char c;
    char resStatus = _responseStatus;

   	inet_ntop(AF_INET, &(m_groupAdrres.sin_addr), ipAddress, INET_ADDRSTRLEN);
	port = ntohs(m_groupAdrres.sin_port);

    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   1 + DELIMITER_LENGHT+ strlen(ipAddress) +DELIMITER_LENGHT + MAX_PORT_LENGHT+DELIMITER_LENGHT  ;
    InitRequst(MESSAGETYPE_CREATE_GROUP,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s%s%s%0.5d%s",c,number,resStatus,DELIMITER,ipAddress,DELIMITER,port,DELIMITER);

    free(number);

return size;

}

/******************************************************/
int Protocol_EncodeGetAllGroups_Response(BackEndStatus _responseStatus, const char* _groupsNames, size_t _numGroups, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   1 + DELIMITER_LENGHT+ strlen(_groupsNames) +DELIMITER_LENGHT + 1  ;
    InitRequst(MESSAGETYPE_GET_ALL_GROUPS,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s%d%s%s",c,number,resStatus,DELIMITER,_numGroups,DELIMITER,_groupsNames);

    free(number);

return size;
}

/**********************************************************/
int Protocol_EncodeLeaveGroup_Response(BackEndStatus _responseStatus, void* _buffer)
{
    int size = 0;
    char c;
    char resStatus = _responseStatus;
    char* number = calloc(TLV_LENGTH_LENGHT+1,sizeof(char));
    int length =   DELIMITER_LENGHT + 1 ;
    InitRequst(MESSAGETYPE_LOGOUT,&c,length,number);
    size =  Tlv_Decode(_buffer,"%c%s%c%s",c,number,resStatus,DELIMITER);

    free(number);
return size;
}


/*******************************static function ***************************/
static void InitRequst(MessageType _type, char* _str,int _lenght, char* _number)
{
    /*conver length to string*/
    sprintf(_number,"%.5d",_lenght);

    /*convert TLV enum to string*/
   *_str = _type;
}
