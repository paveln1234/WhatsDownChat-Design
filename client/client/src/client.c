#include <stdlib.h>
#include <stdio.h>
#include  <string.h>
#include "Client_ui.h"
#include "Client.h"
#include "Net.h"
#include <arpa/inet.h> /* ADDRSELEN */
/* #include <errno.h> */
#define MAGICNUMBER 0xDEADBAEB

struct Client_t
{
    size_t m_magicNumber;
    Socket m_MasterSocket;

};
/************static function****************/
static int ClientInitialize(Client_t* _client,const char* _serverIp, size_t _port);


/************************************************/
Client_t* Client_Create(const char* _serverIp, size_t _port)
{
    int ret = 0;
    Client_t* newClient = NULL;

    if(NULL != _serverIp)
    {
        newClient = malloc(sizeof(Client_t));
        if(NULL == newClient )
            return NULL;

        ret = ClientInitialize(newClient,_serverIp,_port);
        if(!ret)
        {
            free(newClient);
            return NULL;
        }
        newClient->m_magicNumber = MAGICNUMBER;
    }

return newClient;
}

/*********************************************************/
int Client_Send(Client_t* _client,void* _data,size_t _datasize)
{
   return Send(_client->m_MasterSocket,_data,_datasize);
}

/*********************************************************/
int Client_Resive(Client_t* _client,void* _buffer)
{
   return Receive(_client->m_MasterSocket,_buffer,MESSAGE_LENGTH);
}

/*****************************************/
int Client_Destroy(Client_t* _clientToclose)
{
    DisconnectSocket(_clientToclose->m_MasterSocket);
    free(_clientToclose);

return 1;
}

/**********************************************************/
static int ClientInitialize(Client_t* _client,const char* _serverIp, size_t _port)
{
    struct sockaddr_in client_sIn;
    int optval = 1;
    _client->m_MasterSocket = ConnectSocket(_serverIp,_port);
    if(0>_client->m_MasterSocket )
    {
        return 0;
    }
    /* Reusing port */
    if(0 > setsockopt(_client->m_MasterSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval) ) )
    {
        DisconnectSocket(_client->m_MasterSocket);
        return 0;
    }

    memset(&client_sIn , 0 , sizeof(client_sIn) );
    client_sIn.sin_family = AF_INET;
    client_sIn.sin_addr.s_addr = inet_addr(_serverIp);
    client_sIn.sin_port = htons(_port);

    if(0 > connect(_client->m_MasterSocket, (struct sockaddr *) &client_sIn, sizeof(client_sIn)) )
    {
        DisconnectSocket(_client->m_MasterSocket);
        return 0;
    }


return 1;
}

