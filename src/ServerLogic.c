#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Server.h"
#include "Net.h"
#include "Protocol.h"
#include "ServerLogic.h"
#include "Db.h"
#include "IpDb.h"
#include "Config.h"
#define FAIL 0
#define SUCCESS 1

#define MAX_USERS 1024
#define CONFIGFILE "./config/config.ini"

#define MAGICNUMBER 0xDEADBEAB

struct ServerLogic_t
{
        size_t m_magicNumber;
        Db_t* m_usersdb;
        Db_t* m_groupFb;
        IpDb_t* m_ipDb;
        Servet_t* m_server;
        size_t m_maxNumberOfGroups; /*define the size of group db and ip db*/
        size_t m_maxNuberOfUsers;
};

typedef struct GroupDbItem
{
    char* m_GoupName;
    Ip_t* m_groupIp;
    int clients[MAX_USERS] ;
    size_t m_userNumberInTheGroup;
}GroupDbItem;



/****************server static function******************************************/

/** @brief the function add to log new client connection the param as define in server.h */
int NewClietnResive(Socket _socket,struct sockaddr_in* _inCommigSocketInfo,void* _context);
/** @brief the function handel whit new data that receive by  from server param as define in server.h */
int GetData(Socket _socket,void* _data, size_t _datalength,void* _context);
/** @brief the function handel whit disconected client - make logout from all the groups that the client ware connect to them, param as define in server.h */
int DisconectClient(Socket _socket,void* _context);

/** @brief the function fill the param at the ServerInit* _serverinfo
    *@param[in] const char* _filename - file name of the config file
    *@param[out] ServerInit* _serverinfo -server info struct to fill as define in server.h
 */
void IntitServerdata(const char* _filename,ServerInit* _serverinfo);

/** @brief the function init new server logic
    *@param[in] ServerLogic_t* _server -server to init
    *@param[in] size_t _maxNumberOfUsers  - maximum allowd user to connect to server at any moment
    *@param[in] size_t _maxNumberOfGroups - maximum groups that cane be active at server at any moment
 */
static int Logic_Init(ServerLogic_t* _server, size_t _maxNumberOfUsers,size_t _maxNumberOfGroups);

/** @brief the function are destroy key function param as define at db.h*/
static void Destroy_Name(void* _item,void* _contex);
/** @brief the function are destroy item function  paramas define at db.h*/
static void Destroy_Password(void* _item,void* _contex);
/** @brief the function are destroy Group / after the group is no more needed, function  paramas define at db.h*/
static void Destroy_Groupe(void* _item,void* _contex);


/****************static logic  function************************/
/** \brief the function handle whit  Add new user request
                the function add new user to the system
 * \param ServerLogic_t* _server - server to add to him anew user
 * \param const char* _name - new user name
 * \param const char* _password - new user password
 * \return BackEndStatus as define in "server.h"
 */
static BackEndStatus ServerLogic_Signup(ServerLogic_t* _server, const char* _name, const char* _password);

/** \brief the function  handle whit Login  request
                the function make login to the user.
 * \param ServerLogic_t* _server - server to add to him anew user
 * \param const char* _name - user name
 * \param const char* _password -  user password
 * \return BackEndStatus as define in "server.h"
 */
static BackEndStatus ServerLogic_Login(ServerLogic_t* _server, const char* _name, const char* _password );

/** \brief the function handle whit create new group request,
                the function create new group and add the user to the group and receive to user ip adres and port as sockaddr_in_t format
 * \param ServerLogic_t* _server - server to create on him the group
 * \param const char* _groupName - new group name
 * \param[out] sockaddr_in_t* _groupAdrres - socket adress to fiil in him the ip and port
 * \param Socket _socket - socket of the client that want to make new group
 * \return BackEndStatus as define in "server.h"
 */
static BackEndStatus ServerLogic_CreateGroup(ServerLogic_t* _server, const char* _groupName,sockaddr_in_t* _groupAdrres,Socket _socket);

/** \brief the function  handle whit join to group request ,
 *               the function join  user to given group  and receive to user ip adres and port as sockaddr_in_t format
 * \param ServerLogic_t* _server - server to join on him the group
 * \param const char* _groupName -  group name
 * \param[out] sockaddr_in_t* _groupAdrres - socket adress to fiil in him the ip and port
 * \param Socket _socket - socket of the client that want to make new group
 * \return BackEndStatus as define in "server.h"
 */
static BackEndStatus ServerLogic_JoinGroup(ServerLogic_t* _server, const char* _groupName,sockaddr_in_t* _groupAdrres,Socket _socket);

/** \brief the function handle whit receive  all active group request.
 * \param ServerLogic_t* _server - server to get from him the group list
 * \param[out] char* _buffer- buffer to fill in him the groups
 * \param[out]  size_t* number - pointer to size_t the in him will fill the number of the group that recive
 * \return BackEndStatus as define in "server.h"
 */
 static BackEndStatus ServerLogic_GetAllGroups(ServerLogic_t* _server, char* _buffer,size_t* number);

/** \brief the function handle whit logout from group request
 * \param ServerLogic_t* _server - server to get from him the group list
 * \param[out] char* _buffer- buffer to fill in him the groups
 * \param[out]  size_t* number - pointer to size_t the in him will fill the number of the group that recive
 * \return BackEndStatus as define in "server.h"
 */
static BackEndStatus ServerLogic_LogOutGroup(ServerLogic_t* _server, const char* _groupName,Socket _socket);

/**********************main program*****************************************/
/***********************************************************/
ServerLogic_t* ServerLogic_Create(size_t _maxNumberOfUsers,size_t _maxNumberOfGroups)
{
    ServerLogic_t* newServerLogic = NULL;
    if(_maxNumberOfUsers <100 || _maxNumberOfGroups <100)
        return NULL;
    newServerLogic = malloc(sizeof(ServerLogic_t));
    if(NULL == newServerLogic)
        return NULL;
    if(!Logic_Init(newServerLogic,_maxNumberOfUsers,_maxNumberOfGroups))
    {
        free(newServerLogic);
        return NULL;
    }

return newServerLogic;
}

int ServerLogic_Run(ServerLogic_t* _server)
{
    int status = FAIL;
    int serverStatus = 0;
    if(NULL != _server && MAGICNUMBER == _server->m_magicNumber)
    {
        serverStatus = Server_Run(_server->m_server,_server);
        /*if server done the run normally*/
        if(0 < serverStatus )
        {
          status = SUCCESS;
        }
    }

return status;
}

/*********************************************************/
int ServerLogic_Stop(ServerLogic_t* _serverToStop)
{
    int status =FAIL;
    if(NULL != _serverToStop && MAGICNUMBER == _serverToStop->m_magicNumber)
    {
        Server_Stop(_serverToStop->m_server);
        status = SUCCESS;
    }
return status;
}

/*********************************************************/
int ServerLogic_Destroy(ServerLogic_t* _serverToDestroy)
{
    int ret =FAIL;

    if(NULL != _serverToDestroy && MAGICNUMBER == _serverToDestroy->m_magicNumber)
    {
        Server_Close(_serverToDestroy->m_server);
        IpDb_Destroy(_serverToDestroy->m_ipDb);
        Db_Destroy(_serverToDestroy->m_groupFb,Destroy_Name,Destroy_Password);
        Db_Destroy(_serverToDestroy->m_usersdb,Destroy_Name,Destroy_Password);
        _serverToDestroy->m_magicNumber = 0;
        ret =SUCCESS;
    }

 return ret;
}


/*****************static function******************************/
/*********************server logic funcion*************************************************/
static BackEndStatus ServerLogic_Signup(ServerLogic_t* _server, const char* _name, const char* _password)
{
    int ret = 0;
    void** tempItem = NULL;
    char* username = NULL;
    char* password =NULL;
    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _name || NULL == _password)
    {
        return BackEnd_USER_NAME_INVALID;
    }
    ret = Db_GetData(_server->m_usersdb,_name,tempItem);
    if(ret)
        return BackEnd_USER_NAME_TAKEN;
    username = malloc((strlen(_name)+1)*sizeof(char));
    strcpy(username,_name);
    password = malloc((strlen(_name)+1)*sizeof(char));
    strcpy(password,_password);

    ret = Db_Add(_server->m_usersdb,username,(void*)password);
    if(!ret)
    {
        free(username);
        free(password);
        return BackEnd_USER_NAME_TAKEN;
    }

return BackEnd_SUCCESS;
}

/**********************************************************************/
static BackEndStatus ServerLogic_Login(ServerLogic_t* _server, const char* _name, const char* _password )
{
    int ret = 0;
    char* password = NULL;
    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _name || NULL == _password)
    {
        return BackEnd_USER_NAME_INVALID;
    }
	ret = Db_GetData(_server->m_usersdb,_name,(void**)&password);
    if(!ret)
    {
        return BackEnd_USER_NOT_FOUND;
    }

    if(strcmp(_password,password))
    {
        return BackEnd_PASSWORD_INVALID;
    }

return BackEnd_SUCCESS;
}

/**********************************************************************/
static BackEndStatus ServerLogic_CreateGroup(ServerLogic_t* _server, const char* _groupName,sockaddr_in_t* _groupAdrres,Socket _socket)
{
    int ret = 0;
    int i  = 0;
    GroupDbItem* newGroup = NULL;

    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _groupName)
    {
        return BackEnd_GROUP_NAME_INVALID;
    }
    /*Check if the group allrady exists*/
    ret = Db_GetData(_server->m_groupFb, _groupName,(void**)&newGroup);
    if(ret)
    {
        return BackEnd_GROUP_NAME_TAKEN;
    }

    newGroup= malloc(sizeof(GroupDbItem));
    /*Get Ip to the group*/
    for(; i <MAX_USERS ; ++i)
    {
        newGroup->clients[i] = 0;
    }

    newGroup->m_groupIp = IpDb_GetIp(_server->m_ipDb);
    if(NULL == newGroup->m_groupIp)
    {
        free(newGroup);
        return BackEnd_NO_FREE_IP;
    }

    newGroup->m_GoupName = calloc(sizeof(_groupName)+1,sizeof(char));
    strcpy(newGroup->m_GoupName,_groupName);

    /*Add the Group To DB*/
    ret = Db_Add(_server->m_groupFb,newGroup->m_GoupName,newGroup);
    if(!ret)
    {
        return BackEnd_SYSTEM_FAIL;
    }

    /*add the first user to the group and set socket on*/
    ++newGroup->m_userNumberInTheGroup;
    newGroup->clients[_socket] =1 ;

    /*return Group Adress to user*/
    *_groupAdrres = newGroup->m_groupIp->m_Addr;
return BackEnd_SUCCESS;
}

/**********************************************************************/
static BackEndStatus ServerLogic_JoinGroup(ServerLogic_t* _server, const char* _groupName,sockaddr_in_t* _groupAdrres,Socket _socket)
{
    int ret = 0;
    GroupDbItem* temp =NULL;

    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _groupName)
    {
        return BackEnd_GROUP_NAME_INVALID;
    }

	if(1 == temp->clients[_socket])
	{
		return BackEnd_USER_NAME_INVALID;
	}

    ret = Db_GetData(_server->m_groupFb, _groupName,(void**)&temp);
    if(!ret)
    {
        return BackEnd_GROUP_DO_NOT_EXIST;
    }

    /*add the  user to the group counter and set socket on*/
    ++temp->m_userNumberInTheGroup;
    temp->clients[_socket] = 1;
    /*return Group Adress to user*/
    *_groupAdrres = temp->m_groupIp->m_Addr;

return BackEnd_SUCCESS;
}

/**********************************************************************/
static BackEndStatus ServerLogic_LogOutGroup(ServerLogic_t* _server, const char* _groupName,Socket _socket)
{
    int ret = 0;
    GroupDbItem* temp =NULL;

    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _groupName)
    {
        return BackEnd_GROUP_NAME_INVALID;
    }

    ret = Db_GetData(_server->m_groupFb,_groupName,(void**)&temp);
    if(!ret)
    {
        return BackEnd_GROUP_DO_NOT_EXIST;
    }
    /*decrise user from group and reset socket number*/
    --temp->m_userNumberInTheGroup;
    temp->clients[_socket] = 0;

    /*if group empty delete group*/
    if(0 == temp->m_userNumberInTheGroup)
    {
        ret = Db_Remove(_server->m_groupFb, _groupName,(destroyFunction) Destroy_Name,(destroyFunction) Destroy_Groupe,_server);
        if(!ret)
        {
            return BackEnd_SYSTEM_FAIL;
        }
    }
return BackEnd_SUCCESS;
}


/**********************************************************************/
static BackEndStatus ServerLogic_GetAllGroups(ServerLogic_t* _server, char* _buffer,size_t* number)
{

    int ret = 0;
    if(NULL == _server || MAGICNUMBER != _server->m_magicNumber || NULL == _buffer)
    {
        return BackEnd_GROUP_NAME_INVALID;
    }

    ret = Db_GetAllItems(_server->m_groupFb,_buffer,DELIMITER);

    *number = ret;

return BackEnd_SUCCESS;

}


/****************server function******************************/
int DisconectClient(Socket _socket,void* _context)
{
    /**TODO
        add log out sock num from all grups
    */
    printf("socket number %d disconnect : \n",_socket);
	return 1;
}

/****************************************************************/
int NewClietnResive(Socket _socket,struct sockaddr_in* _inCommigSocketInfo,void* _context)
{
    printf("socket number %d connect : \n",_socket);
	return 1;
}

/****************************************************************/
int GetData(Socket _socket,void* _data, size_t _datalength,void* _context)
{
	char returnMsg[MAX_MESSAGE_LENGTH] = {0};
	int dataLength = 0;
	char* data = _data;
	ServerLogic_t* serverLogic = (ServerLogic_t*)_context;
	sockaddr_in_t tempAdres;

    ClientReceiveMessage_t* newMessage = calloc(1,sizeof(ClientReceiveMessage_t));
    ServerReceiveMessage_t* newmsg = calloc(1,sizeof(ServerReceiveMessage_t));

    Protocol_DecodeServer(data,_datalength,newmsg);

        printf("\n RESIVEMESAGE - type %c -- username   %s -- password   %s -- groupname %s\n",newmsg->m_MessageType ,newmsg->m_userName,newmsg->m_password,newmsg->m_groupName);

    switch(newmsg->m_MessageType)
    {
        case MESSAGETYPE_SIGNUP:
                    {
                        newMessage->m_status = ServerLogic_Signup(serverLogic,newmsg->m_userName,newmsg->m_password);
                        dataLength = Protocol_EncodeSignUp_Response(newMessage->m_status,returnMsg);
                        break;
                    }
        case MESSAGETYPE_LOGIN:
                    {
                        newMessage->m_status = ServerLogic_Login(serverLogic,newmsg->m_userName,newmsg->m_password);
                        dataLength = Protocol_EncodeLogIn_Response(newMessage->m_status,returnMsg);
                        break;
                    }
        case MESSAGETYPE_LOGOUT:
        case MESSAGETYPE_DELETE_USER:
        case MESSAGETYPE_JOIN_GROUP:
                    {
                        newMessage->m_status = ServerLogic_JoinGroup(serverLogic,newmsg->m_groupName,&tempAdres,_socket);
                        dataLength = Protocol_EncodeJoinGroup_Response(newMessage->m_status,tempAdres,returnMsg);
                        break;
                    }
        case MESSAGETYPE_CREATE_GROUP:
                    {
                        newMessage->m_status = ServerLogic_CreateGroup(serverLogic,newmsg->m_groupName,&tempAdres,_socket);
                        dataLength = Protocol_EncodeNewGroup_Response(newMessage->m_status,tempAdres,returnMsg);
                        break;
                    }
        case MESSAGETYPE_LEAVE_GROUP:
                    {
                        newMessage->m_status = ServerLogic_LogOutGroup(serverLogic, newmsg->m_groupName, _socket);
                        dataLength = Protocol_EncodeLeaveGroup_Response(newMessage->m_status,returnMsg);

                    }
        case MESSAGETYPE_GET_ALL_GROUPS:
                    {
                        char groups[MAX_MESSAGE_LENGTH]={0};
                        size_t number = 0;
                        newMessage->m_status = ServerLogic_GetAllGroups(serverLogic,groups,&number);
                        dataLength = Protocol_EncodeGetAllGroups_Response(newMessage->m_status,groups,number,returnMsg);
                    }
        default:
        {
            break;
        }
    }

    printf("SENDBACKTOUSER ---- %s\n",returnMsg);
	if(0 > Server_Send(_socket,returnMsg,dataLength))
	{
		printf("error send");
	}
	free(newMessage);
	free(newmsg);

return 0;
}


/******************static function for server's action function***************************/
static void Destroy_Name(void* _item,void* _contex)
{
    free(_item);
}

/*********************************************/
static void Destroy_Password(void* _item,void* _contex)
{
    free(_item);
}

/*********************************************/
static void Destroy_Groupe(void* _item,void* _contex)
{
    ServerLogic_t* server = (ServerLogic_t*)_contex;
    GroupDbItem* temp = (GroupDbItem*)_item;
    IpDb_ReturnIp(server->m_ipDb,temp->m_groupIp);
    free(temp);
}

/****************static function for server init*******************************************/
void IntitServerdata(const char* _filename,ServerInit* _serverinfo)
{
    ConfigStuct* config =NULL;
    config = Config_Get(_filename);
	_serverinfo->m_ip = calloc(strlen(config->m_ip)+1,sizeof(char));
	 strcpy(_serverinfo->m_ip,config->m_ip);

	_serverinfo->m_port = config->m_port;
	_serverinfo->m_maxNumberOfClient = config->m_maxNumberOfClient;
	_serverinfo->m_waitingListSize = config->m_waitingListSize;
	_serverinfo->m_acceptFunction = &NewClietnResive;
	_serverinfo->m_recieveDatafunction = &GetData;
    _serverinfo->m_disconectClientFunc = &DisconectClient;

}

/********************Server Logic Init**************************************************/
static int Logic_Init(ServerLogic_t* _server, size_t _maxNumberOfUsers,size_t _maxNumberOfGroups)
{
    ServerInit* serverinfo = NULL;
    serverinfo = calloc(1,sizeof(ServerInit));
	IntitServerdata(CONFIGFILE,serverinfo);

    if(NULL == (_server->m_usersdb =Db_Creare(serverinfo->m_maxNumberOfClient)))
    {
        return FAIL;
    }

    if(NULL == (_server->m_groupFb =Db_Creare(_maxNumberOfGroups)))
    {
        Db_Destroy(_server->m_usersdb,Destroy_Name,Destroy_Password);
        return FAIL;
    }

    if(NULL == (_server->m_ipDb =IpDb_Creare(_maxNumberOfGroups)))
    {
        Db_Destroy(_server->m_usersdb,Destroy_Name,Destroy_Password);
        Db_Destroy(_server->m_groupFb,Destroy_Name,Destroy_Password);
        return FAIL;
    }


	_server->m_server = Server_Initialize(serverinfo);
    if(NULL == _server->m_server)
	{
        Db_Destroy(_server->m_usersdb,Destroy_Name,Destroy_Password);
        Db_Destroy(_server->m_groupFb,Destroy_Name,Destroy_Password);
        IpDb_Destroy(_server->m_ipDb);
		free(serverinfo->m_ip);
		free(serverinfo);
		return FAIL;
	}

	free(serverinfo->m_ip);
	free(serverinfo);
    _server->m_maxNuberOfUsers = _maxNumberOfUsers;
    _server->m_maxNumberOfGroups =_maxNumberOfGroups;
    _server->m_magicNumber = MAGICNUMBER;

return SUCCESS;
}
