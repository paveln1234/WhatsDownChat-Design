#include <stdio.h>/*file*/
#include <stdlib.h>
#include <string.h>
#include "Config.h"
#include "hashmap.h"
#define NUMBEROFLINES 20
#define BUFFER 256
#define NAME_SIZE 64
#define VAL_SIZE 64
/*config ini file struct [item] = val  */

/***************static function**************************************/
/*****************************************************/
/*the function parse agiven filr in to ConfigStruct as define in "config.h file*/
static int ParseFile(FILE* _fp,ConfigStuct* _config);
/** @brief the function create new name and val from taken line
 * @param[out] - char* _name - the function return name throw the pointer
 * @param[out] - char* _val - the function return val throw the pointer
    @note - the function make malloc for name and val, user respons to free.
*/
static void GetNameAndValFromLine(const char* _line,char* _name, char* _val);

/****************hashmap function*************************/
static size_t hashfunc(char* _str);
/*the function compare two val names*/
static  int IsEqual(char* _firstKey,char* _secondKey);
static void Destroy_item(void* _item);

/****************************************************/
ConfigStuct* Config_Get(const char* _filename)
{
	ConfigStuct* newConfig;
	FILE* file;
    int retVal=0;

	if(NULL == _filename)
	{
			perror("no file name:");

		return NULL;
	}

    newConfig = malloc(sizeof(ConfigStuct));
	file = fopen(_filename,"r+");
	if(0>file)
	{
		perror("CONFIG FILE OPEN ERROR --USING DEFULE CONFIG:");
		strcpy(newConfig->m_ip , DEFULT_IP);
        newConfig->m_port = DEFULT_PORT;
        newConfig->m_maxNumberOfClient = DEFULT_MAX_CLIENT;
        newConfig->m_waitingListSize = DEFULT_WATING_LIST;
        newConfig->m_maxUsers = DEFULT_MAX_USERS;
        newConfig->m_maxGroups = DEFULT_MAX_GROUPS;
		return newConfig;
	}



     retVal  = ParseFile(file,newConfig);
	if(!retVal)
    {
        perror("CONFIG FILE READ ERROR --USING DEFULE CONFIG:");
		strcpy(newConfig->m_ip , DEFULT_IP);
        newConfig->m_port = DEFULT_PORT;
        newConfig->m_maxNumberOfClient = DEFULT_MAX_CLIENT;
        newConfig->m_waitingListSize = DEFULT_WATING_LIST;
        newConfig->m_maxUsers = DEFULT_MAX_USERS;
        newConfig->m_maxGroups = DEFULT_MAX_GROUPS;
		return newConfig;
    }
	fclose(file);
return newConfig;
}

/****************************************************/
int Config_Destroy(ConfigStuct* _configStuctToDestroy)
{
    free(_configStuctToDestroy);

return 1;
}



/*****************************************************/
static int ParseFile(FILE* _fp,ConfigStuct* _config)
{

    HashMap* temp;
    char line[256] = {0};
	char* name ;
	char* val ;
    char* ip;
    char* port;
    char* maxclients;
    char* maxWaitList;
    char* maxUsers;
    char* maxGroups;
    int retVal = 1;

    temp = HashMap_Create(NUMBEROFLINES,(HashFunction) hashfunc, (EqualityFunction) IsEqual);
    if(NULL == temp)
    {
        return 0;
    }

    while(fgets(line,BUFFER,_fp) != NULL && !ferror(_fp))
    {
         

            name = calloc(NAME_SIZE,sizeof(char));
            val = calloc(VAL_SIZE,sizeof(char));
            GetNameAndValFromLine(line,name,val);
            if(MAP_SUCCESS != HashMap_Insert(temp,(void*)name,(void*) val))
            {
                free(name);
                free(val);
                return 0;
            }
    }
    if(MAP_SUCCESS != HashMap_Find(temp,"ip",(void**)&ip))
   {
            strcpy(_config->m_ip , DEFULT_IP);
            retVal = 0;
   }
    else
    {
        strcpy(_config->m_ip ,name);
    }
    if(MAP_SUCCESS != HashMap_Find(temp,"port",(void**)&port))
   {
            _config->m_port = DEFULT_PORT;
            retVal = 0;
   }
   else
   {
       _config->m_port = atoi(port);
   }

    if(MAP_SUCCESS != HashMap_Find(temp,"max_conection",(void**)&maxclients))
   {
        _config->m_maxNumberOfClient = DEFULT_MAX_CLIENT;
        retVal = 0;
   }
   else
   {
       _config->m_maxNumberOfClient = atoi(maxclients);
   }

    if(MAP_SUCCESS != HashMap_Find(temp,"waiting_list",(void**)&maxWaitList))
    {
        _config->m_waitingListSize = DEFULT_WATING_LIST;
        retVal = 0;
    }
       else
   {
       _config->m_waitingListSize = atoi(maxWaitList);
   }

    if(MAP_SUCCESS != HashMap_Find(temp,"waiting_list",(void**)&maxGroups))
    {
        _config->m_maxGroups = DEFULT_MAX_GROUPS;
        retVal = 0;
    }
       else
   {
       _config->m_maxGroups = atoi(maxGroups);
   }

    if(MAP_SUCCESS != HashMap_Find(temp,"waiting_list",(void**)&maxUsers))
    {
        _config->m_maxUsers = DEFULT_MAX_USERS;
        retVal = 0;
    }
       else
   {
       _config->m_maxUsers = atoi(maxUsers);
   }

    HashMap_Destroy(&temp,Destroy_item,Destroy_item);

return retVal;
}

static void Destroy_item(void* _item)
{
    free(_item);
}

static void GetNameAndValFromLine(const char* _line,char* _name, char* _val)
{
    char name[32] = {0};
    char val[32] = {0};
    int i =0;
    while( '[' != *_line)
    {
        ++_line;
    }

    ++_line;

    while( ']' != *_line)
    {
        name[i] = *_line;
        ++_line;
        ++ i;
    }

    while( '=' != *_line)
    {
        ++_line;
    }
      strcpy(_name,name);
    ++_line;
    i = 0;

    while(' ' == *_line )
    {
        ++_line;
    }

    while('\0' != *_line && ' ' != *_line  &&  '\n' != *_line)
    {
        val[i] = *_line;
        ++_line;
        ++ i;
    }
      strcpy(_val,val);

return;
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

