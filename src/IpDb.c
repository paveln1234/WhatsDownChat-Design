#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* strcpy()*/
#include <arpa/inet.h>

#include "IpDb.h"
#include "queue.h"
#define MAGICNUMBER 0xDEADBDDD
struct IpDb_t
{
    size_t  m_magicNumber;
    Queue* m_ipDb;
    size_t m_maxNumberOfIp;
};

/********static function**********/
static void DestroyElement(void* _element);

/************************************************/
IpDb_t* IpDb_Creare(size_t _dbSize)
{
    int i = 2;
    char udpAdress[12]= "238.000.000";
    size_t port = 7254;
    char ipAddress[INET_ADDRSTRLEN];
    Ip_t* newip;
    IpDb_t* newDb;

    newDb = malloc(sizeof(IpDb_t));
    if(NULL == newDb)
        return NULL;

    newDb->m_ipDb = Queue_Create();
    if(NULL == newDb)
        return NULL;
    for(;i<_dbSize+2;++i)
    {
        newip =malloc(sizeof(Ip_t));
        newip->m_Addr.sin_family = AF_INET;
        newip->m_Addr.sin_port = htons(port);
        sprintf(ipAddress,"%s.%.3d",udpAdress,i);

        inet_aton(ipAddress, &newip->m_Addr.sin_addr);
        if(QUEUE_SUCCESS != Queue_Enqueue(newDb->m_ipDb,(void*)newip))
        {
            Queue_Destory(&newDb->m_ipDb,DestroyElement);
            free(newDb);
        }
    }
    newDb->m_maxNumberOfIp = _dbSize;
    newDb->m_magicNumber = MAGICNUMBER;

return newDb;
}

/********************************************/
Ip_t* IpDb_GetIp(IpDb_t* _db)
{
    Ip_t* temp;
    if(NULL == _db || MAGICNUMBER != _db->m_magicNumber)
    {
        return NULL;
    }

    if(QUEUE_SUCCESS != Queue_Dequeue(_db->m_ipDb,(void**)&temp))
    {
        return NULL;
    }

return temp;
}

/********************************************/
int IpDb_ReturnIp(IpDb_t* _db,Ip_t* _returnIp)
{
    if(NULL == _db || MAGICNUMBER != _db->m_magicNumber)
    {
        return 0;
    }

    if(QUEUE_SUCCESS != Queue_Enqueue(_db->m_ipDb,(void*)_returnIp))
    {
        return 0;
    }

return 1;
}

/*****************************************/
int IpDb_Destroy(IpDb_t* _dbToDistroy)
{
    if(NULL == _dbToDistroy || MAGICNUMBER != _dbToDistroy->m_magicNumber)
        return 0;

    Queue_Destory(&_dbToDistroy->m_ipDb,DestroyElement);
    _dbToDistroy->m_magicNumber = 0;
    free(_dbToDistroy);

return 1;
}

/********************************************/
static void DestroyElement(void* _element)
{
    free(_element);
}
