#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include "Db.h"
#include"hashmap.h"
#include "ClientLogic.h"
#include "Client.h"
#include "Protocol.h"
#include "Client_ui.h"
#define NICKNAME "pavel"
#define MAGICNUMBER 0xDEADDEAD
#define MAX_GROUPS_FOR_USER 100
#define FIFO_PATH "./msgfifo"

struct ClientLogic_t
{
    size_t m_magicNumber;
    int m_fifo;
    Client_t* m_client;
    Client_Ui* m_ui;
    char* m_userName;
    Db_t* m_clientGroups;
};

typedef struct GroupsStruct
{
    pid_t m_reader;
    pid_t m_writer;
}GroupsStruct;

/*************static function****************/
static ClientRecieveStatus UserHandeler(UiRequst _selct,char* _param1, char* _param2,void* _contxt);
static ClientRecieveStatus ServerAnsToUiStatus(BackEndStatus _ans);
static void Destroy_Group(void* _item,void* _contex);

/*static int NewChat(char* _ip, char* _port, char* _groupName, char* _nickname );*/
/************************************************/
ClientLogic_t* clientLogic_Create(const char* _serverIp,size_t _port)
{
    ClientLogic_t* newClient = NULL;

	if(0 >mkfifo(FIFO_PATH,0666))
    {
        perror("FIFO not make");
    }

    if(NULL != _serverIp && 1024 < _port)
    {
        newClient = calloc(1,sizeof(ClientLogic_t));
        if(NULL == newClient )
            return NULL;
        if(0 > (newClient->m_fifo = open(FIFO_PATH,O_RDONLY|O_NONBLOCK)))
        {
            free(newClient);
            return NULL;
        }

        newClient->m_ui = Ui_Create((actionFunction)UserHandeler);
        if(NULL == newClient->m_ui)
        {
            free(newClient);
            return NULL;
        }

        if(0 == (newClient->m_client = Client_Create(_serverIp,_port)))
       {
           Ui_Destroy(newClient->m_ui);
           free(newClient);
           return NULL;
        }
        if(NULL == (newClient->m_clientGroups = Db_Creare(MAX_GROUPS_FOR_USER)))
        {
            Client_Destroy(newClient->m_client);
            Ui_Destroy(newClient->m_ui);
           free(newClient);
           return NULL;
        }
    }
    newClient->m_magicNumber = MAGICNUMBER;

return newClient;
}

/*********************************************/
int clientLogic_Run(ClientLogic_t* _clientToRun)
{
     Ui_Run(_clientToRun->m_ui,(void*)_clientToRun);
     return 1;
}
/*********************************************/
int clientLogic_Stop(ClientLogic_t* _clientToStop)
{
    if(NULL == _clientToStop || MAGICNUMBER != _clientToStop->m_magicNumber)
    {
        return 0;
    }

    Client_Destroy(_clientToStop->m_client);
    Ui_Destroy(_clientToStop->m_ui);
    Db_Destroy(_clientToStop->m_clientGroups,Destroy_Group,Destroy_Group);
    _clientToStop->m_magicNumber = 0;
    free(_clientToStop);

return 1;
}

/**********************************************/
int clientLogic_Destroy(ClientLogic_t* _clientToDestroy)
{
    if(NULL == _clientToDestroy || MAGICNUMBER != _clientToDestroy->m_magicNumber)
    {
        return 0;
    }
    close(_clientToDestroy->m_fifo);
    unlink(FIFO_PATH);
    Client_Destroy(_clientToDestroy->m_client);
    Ui_Destroy(_clientToDestroy->m_ui);
    _clientToDestroy->m_magicNumber = 0;
    free(_clientToDestroy);
return 1;
}


/***************static function*******************************/
static ClientRecieveStatus UserHandeler(UiRequst _selct,char* _param1, char* _param2,void* _contxt)
{
    char buffer[BUFFER_SIZE] = {0};
    ClientReceiveMessage_t* Message =malloc(sizeof(ClientReceiveMessage_t));
    ClientLogic_t* clientLogic = (ClientLogic_t*)_contxt;
    GroupsStruct* newGroup =NULL;
    char* newGroupName = NULL;
    size_t msgSize = 0;

    switch(_selct)
    {
        case UI_SIGNUP:
             {
                 msgSize = Protocol_EncodeSignUp(_param1,_param2,buffer);
                /*save client name*/
                clientLogic->m_userName = calloc((strlen(_param1)+1),sizeof(char));
                strcpy(clientLogic->m_userName,_param1);

                Client_Send(clientLogic->m_client,buffer,msgSize);
                memset(buffer,0,msgSize);
                msgSize = Client_Resive(clientLogic->m_client,buffer);
                Protocol_DecodeClient(buffer,msgSize,Message);
                return ServerAnsToUiStatus(Message->m_status);
             }
        case UI_LOGIN:
            {
                msgSize = Protocol_EncodeLogIn(_param1,_param2,buffer);
                clientLogic->m_userName = calloc((strlen(_param1)+1),sizeof(char));
                strcpy(clientLogic->m_userName,_param1);

                Client_Send(clientLogic->m_client,buffer,msgSize);
                memset(buffer,0,msgSize);
                msgSize = Client_Resive(clientLogic->m_client,buffer);
                Protocol_DecodeClient(buffer,msgSize,Message);
                return ServerAnsToUiStatus(Message->m_status);
            }

        case UI_NEW_GROUP:
            {
                if(Db_IsInDb(clientLogic->m_clientGroups,_param1))
                {
                    return CLIENT_STATUS_BAD_PARAM;
                }
                msgSize = Protocol_EncodeNewGroup(_param1,buffer);
                Client_Send(clientLogic->m_client,buffer,msgSize);
                memset(buffer,0,msgSize);
                msgSize = Client_Resive(clientLogic->m_client,buffer);
                Protocol_DecodeClient(buffer,msgSize,Message);
                if(BackEnd_SUCCESS == Message->m_status)
                {
                        newGroup = malloc(sizeof(GroupsStruct));
                        newGroupName = calloc(strlen(_param1)+1,sizeof(char));
                        strcpy(newGroupName,_param1);
                        char writerCommand[100];
                        char readerCommand[100];
                        sprintf(writerCommand, "gnome-terminal --geometry=100x10+0+600 --command=\"./writerapp/writer %s %s %s %c", Message->m_ip, Message->port,clientLogic->m_userName,'\"');
                        sprintf(readerCommand, "gnome-terminal --geometry=100x30+0+0 --command=\" ./readerapp/reader %s %s %s%c",  Message->m_ip, Message->port,_param1,'\"');
                        system(readerCommand);
                        system(writerCommand);
                        read(clientLogic->m_fifo, (void*)&newGroup->m_reader, sizeof(pid_t));
                        read(clientLogic->m_fifo, (void*)&newGroup->m_writer, sizeof(pid_t));
                        Db_Add(clientLogic->m_clientGroups,newGroupName,newGroup);
                }
                return ServerAnsToUiStatus (Message->m_status);
                break;
            }
        case UI_JOIN_GRUP:
            {
                if(Db_IsInDb(clientLogic->m_clientGroups,_param1))
                {
                    return CLIENT_STATUS_ALRDY_IN_GROUP;
                }
                msgSize = Protocol_EncodeJoinGroup(_param1,buffer);
                Client_Send(clientLogic->m_client,buffer,msgSize);
                memset(buffer,0,msgSize);
                msgSize = Client_Resive(clientLogic->m_client,buffer);
                Protocol_DecodeClient(buffer,msgSize,Message);
                if(BackEnd_SUCCESS == Message->m_status)
                {

                        newGroup = malloc(sizeof(GroupsStruct));
                        newGroupName = calloc(strlen(_param1)+1,sizeof(char));
                        strcpy(newGroupName,_param1);
                        char writerCommand[100];
                        char readerCommand[100];
                        sprintf(writerCommand, "gnome-terminal --geometry=100x10+0+600 --command=\"./writerapp/writer %s %s %s %c", Message->m_ip, Message->port,"pavel",'\"');
                        sprintf(readerCommand, "gnome-terminal --geometry=100x30+0+0 --command=\" ./readerapp/reader %s %s %s %c",  Message->m_ip, Message->port,_param1,'\"');

                        system(writerCommand);
                        system(readerCommand);
                        read(clientLogic->m_fifo, (void*)&newGroup->m_reader, sizeof(pid_t));
                        read(clientLogic->m_fifo, (void*)&newGroup->m_writer, sizeof(pid_t));
                        Db_Add(clientLogic->m_clientGroups,newGroupName,newGroup);
            }
                return ServerAnsToUiStatus(Message->m_status);
                break;
            }
            case UI_GET_ALL_GROUPS:
                    {
                        msgSize = Protocol_EncodeGetAllGroups(buffer);
                        Client_Send(clientLogic->m_client,buffer,msgSize);
                        memset(buffer,0,msgSize);
                        msgSize = Client_Resive(clientLogic->m_client,buffer);
                        Protocol_DecodeClient(buffer,msgSize,Message);
                        strcpy(_param1,Message->m_data);
                        return ServerAnsToUiStatus(Message->m_status);
                        break;
                    }
            case UI_LEAVE_GROUP:
                    {
                        if(!Db_GetData(clientLogic->m_clientGroups,_param1,(void**)&newGroup))
                        {
                            return CLIENT_STATUS_GROUP_DO_NOT_EXIST;
                        }
                        msgSize = Protocol_EncodeLeaveGroup(_param1,buffer);
                        Client_Send(clientLogic->m_client,buffer,msgSize);
                        memset(buffer,0,msgSize);
                        msgSize = Client_Resive(clientLogic->m_client,buffer);
                        Protocol_DecodeClient(buffer,msgSize,Message);
                        kill(newGroup->m_reader,SIGKILL);
                        kill(newGroup->m_writer,SIGKILL);
                        return ServerAnsToUiStatus(Message->m_status);
                        break;

                    }
            default:
                {
                    return CLIENT_STATUS_OK;
                    break;
                }

    }

return ServerAnsToUiStatus (Message->m_status);
}
/******************************************************/
 static ClientRecieveStatus ServerAnsToUiStatus(BackEndStatus _ans)
 {
        switch(_ans)
        {
            case BackEnd_SUCCESS:
                {
                    return CLIENT_STATUS_OK;
                    break;
                }
            case BackEnd_PASSWORD_INVALID:
                {
                    return CLIENT_STATUS_BAD_PARAM;
                    break;
                }
            case BackEnd_SYSTEM_FAIL:
            case BackEnd_UNKNOWN_ERROR:
            case BackEnd_REQUST_UNKNOWN:
                {
                    return CLIENT_STATUS_SERVER_FALL;
                    break;
                }

            case BackEnd_USER_NOT_FOUND:
                {
                    return CLIENT_STATUS_USER_NOT_FOUND;
                    break;
                }
            case BackEnd_USER_NAME_INVALID:
            case BackEnd_GROUP_NAME_INVALID:
            case BackEnd_GROUP_DO_NOT_EXIST:
                {
                    return 	CLIENT_STATUS_GROUP_DO_NOT_EXIST;
                    break;
                }

            case BackEnd_USER_NAME_TAKEN:
            case BackEnd_GROUP_NAME_TAKEN:
                {
                    return CLIENT_STATUS_NAME_TAKEN;
                    break;
                }
            case BackEnd_NO_FREE_IP:
                {
                    return CLIENT_STATUS_NO_FREE_IP;
                    break;
                }

            case BackEnd_CLIENT_DISCONNECTED:
                {
                    return CLIENT_STATUS_SERVER_FALL;
                }
            default:
                return CLIENT_STATUS_BAD_PARAM;
        }
 }

/*********************************************/
static void Destroy_Group(void* _item,void* _contex)
{
    free(_item);
}
