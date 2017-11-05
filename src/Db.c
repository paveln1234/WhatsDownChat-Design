#include <stdio.h> /*sprintf*/
#include <stdlib.h>/*calloc*/
#include <string.h>/*strcmp*/
#include "Db.h"
#include "hashmap.h"
#define MAGICNUMBER 0xDEADBEAB
typedef  void (*DestroyfuncHash)(void* _key);

struct Db_t
{
    size_t m_magicNumber;
    HashMap* m_Db;
    size_t m_maxUserNumber;
    size_t m_correcrUserNamber;
};

typedef struct BufferAndLenght
{
    char* m_buffer;
    char* m_delimeter;
    size_t m_offset;
    size_t m_counter;
}BufferAndLenght;


/****************static function**********************/
static  int AddDelemeterToValue(const void* _key, void* _value, void* _context);
/****************hashmap function*************************/
static size_t hashfunc(char* _str);
/*the function compare to user names*/
static  int IsEqual(char* _firstKey,char* _secondKey);

/********************************************/
Db_t* Db_Creare(size_t _dbSize)
{
    Db_t* newDb =NULL;
    /*check if _dbsize  >= 100 if nut  return null*/
    if (100 >_dbSize)
    {
        return NULL;
    }

    newDb = calloc(1,sizeof(Db_t));
    if(NULL == newDb)
    {
        return NULL;
    }
    newDb->m_Db =HashMap_Create(_dbSize,(HashFunction) hashfunc, (EqualityFunction) IsEqual);
    if(NULL == newDb->m_Db)
    {
        free(newDb);
        return NULL;
    }
    newDb->m_maxUserNumber = _dbSize;
    newDb->m_correcrUserNamber = 0;
    newDb->m_magicNumber = MAGICNUMBER;

return newDb;
}

/*********************************************************/
int Db_Add(Db_t* _db,const char* _key, void* _data)
{
    int ret =0;
    if(NULL != _db &&  NULL != _key && NULL != _data && MAGICNUMBER == _db->m_magicNumber && (_db->m_maxUserNumber >= _db->m_correcrUserNamber +1))
    {
        if(MAP_SUCCESS == HashMap_Insert(_db->m_Db,(void*)_key,_data))
           {
               ret =1;
               ++_db->m_correcrUserNamber;
           }
    }

return ret;
}

/*********************************************************/
int Db_Remove(Db_t* _db,const char* _key,destroyFunction _destroyKeyFunction,destroyFunction _destroyItemFunction,void* _context)
{
    int ret =0;
    void* returnDataBuffer;
    char* retKey;
    if(NULL != _db &&  NULL != _key &&   MAGICNUMBER == _db->m_magicNumber && (_db->m_maxUserNumber >= _db->m_correcrUserNamber +1))
    {
        if(MAP_SUCCESS == HashMap_Remove(_db->m_Db,(void*)_key,(void**)&retKey, (void**)&returnDataBuffer))
       {
           ret =1;
           --_db->m_correcrUserNamber;
           if(0 == _db->m_correcrUserNamber)
           {
                _destroyItemFunction(retKey,_context);
                _destroyItemFunction(returnDataBuffer,_context);
               return -1;
           }
       }
    }

return ret;
}

/*********************************************************/
int Db_GetData(Db_t* _db,const char* _key, void** _returnDataBuffer )
{
    int ret =0;
    if(NULL != _db &&  NULL != _key && NULL != _returnDataBuffer && MAGICNUMBER == _db->m_magicNumber && 0 !=_db->m_correcrUserNamber)
    {
        if(MAP_SUCCESS == HashMap_Find(_db->m_Db,(void*)_key,_returnDataBuffer))
       {
           ret =1;
       }
    }

return ret;
}

/*********************************************************/
int Db_Destroy(Db_t* _dbToDistroy,destroyFunction _destroyKeyFunction,destroyFunction _destroyItemFunction )
{
    int ret =0;
    if(NULL != _dbToDistroy &&   _dbToDistroy->m_magicNumber)
    {
        HashMap_Destroy(&_dbToDistroy->m_Db,(DestroyfuncHash)_destroyKeyFunction,(DestroyfuncHash)_destroyItemFunction);
           ret =1;
           _dbToDistroy->m_correcrUserNamber = 0;
           _dbToDistroy->m_magicNumber = 0;
    }

return ret;
}

int Db_IsInDb(Db_t* _db,const char* _key)
{
    int ret = 0;
    void ** temp = NULL;
    if(NULL != _db &&  NULL != _key && _db->m_magicNumber)
    {
        if(MAP_SUCCESS == HashMap_Find(_db->m_Db,_key,temp))
       {
           ret = 1;
       }
    }

return ret;
}

/***********************************************************/
int Db_GetAllItems(Db_t* _db, void* _buffer, char* _demitted)
{
    BufferAndLenght* bANDl = NULL ;
    int ret =0;
    if(NULL != _db &&  NULL != _buffer && NULL != _demitted && _db->m_magicNumber)
    {
        bANDl = calloc(1,sizeof(BufferAndLenght));
        bANDl->m_offset = 0;
        bANDl->m_counter = 0;
        bANDl->m_buffer = _buffer;
        bANDl->m_delimeter = _demitted;

       ret =  HashMap_ForEach(_db->m_Db,(KeyValueActionFunction)AddDelemeterToValue,(void*)bANDl);
        ret = bANDl->m_counter;
       free(bANDl);
    }

return ret;
}


/****************hashmap function*************************/
static size_t hashfunc(char* _str)
{
    size_t hash = 5381;
    size_t i = 0;
    size_t len = strlen(_str);
    for(i = 0; i < len; _str++, i++)
    {
        hash = ((hash << 5) + hash) + (*_str);
    }

    return hash;
}

/***************************************************/
static  int IsEqual(char* _firstKey,char* _secondKey)
{
	return (strcmp(_firstKey,_secondKey) == 0 ? 1 : 0);
}

/****************************************************/
static int AddDelemeterToValue(const void* _key, void* _value, void* _context)
{

    char* item = (char*)_key;

    BufferAndLenght* bANDl = (BufferAndLenght*)_context;

    bANDl->m_offset += sprintf(bANDl->m_buffer+bANDl->m_offset,"%s%s",item,bANDl->m_delimeter);
    ++bANDl->m_counter;

return 1;
}
