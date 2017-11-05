#include <stdio.h>
#include <stdlib.h>/*for malloc*/
#include <math.h> /*for sqrt*/
#include "../inc/vector.h"
#include "../inc/hashmap.h"
#include "../inc/macros.h"
#include "../inc/list_functions.h"
#include "../inc/list_itr.h"
#include "../inc/list.h"

#define MAGICNUMBER 0xDEADBABE
#define TRUE 1
#define FALSE 0

typedef void (*removVal)(void* _elemnt);
typedef void (*checkEqultyKey)(EqualityFunction Equality,void* m_key);


/******************************************/

typedef struct elementsRemover
{
	removVal m_destroyKey;
	removVal m_destroyValue;

} elementsRemover;

typedef struct ListItem
{
	void* m_key;
	void* m_value;

} ListItem;

typedef struct searchandkey
{

 EqualityFunction   m_equality;
 void* m_key;

}searchandkey;

struct HashMap
{
    Vector_t* m_vector;
    size_t   m_Size;
    size_t   m_NumberOfElements;
    size_t   m_MagicNumber;
    HashFunction     m_HashFunc;
    EqualityFunction     m_Equality;

};

typedef struct actionandfunction
{
    KeyValueActionFunction action;
    void* contex;

}actionandfunction;

/*internal function*/
static size_t NextPrimeNumber(size_t _size);
static size_t isPrime(size_t _number);
static void removItemFromList(ListItem* _item,elementsRemover* elementsRemover);
static int searchduplic(List* _list,searchandkey* _searchkey, void** _ppitem,void** _ppListItr);
static size_t getIndex(const HashMap* _map , const void* _searchKey);
int seperadeKeyFromItem(ListItem* _item,searchandkey* _searchkey);
static Vector_t* MoveItemsFromOldVectorToNewVector(HashMap* _map,size_t oldSize);
static int revVal(ListItem* _item, actionandfunction* _action);



HashMap* NOTHashMap_Create(size_t _capacity, HashFunction _hashFunc, EqualityFunction _keysEqualFunc)
{
    HashMap* newHash = NULL;
    Vector_t* newVec;
    List* list;
    size_t number;
    size_t i;

    CHECK_EQUAL_RETURN(0,_capacity,NULL);
    CHECK_EQUAL_RETURN(NULL,_hashFunc,NULL);
    CHECK_EQUAL_RETURN(NULL,_keysEqualFunc,NULL);

    newHash = (HashMap*)malloc(sizeof(HashMap));
    CHECK_EQUAL_RETURN(NULL,newHash,NULL);

    number = NextPrimeNumber(_capacity);
    newVec = Vector_Create(number,0);
    if(NULL == newVec)
    {
        free(newHash);
        return NULL;
    }

    /*create all lists from the begin to avoid the creaty and the cheeking in the future */
    for( i = 1; i <= number ;++i)
    {
        list = List_Create();
       CHECK_NOTEQUAL_RETURN(VECTOR_SUCCESS,Vector_Append(newVec,list),NULL);
    }

    newHash->m_vector = newVec;
    newHash->m_Size = number;
    newHash->m_NumberOfElements = 0;
    newHash->m_MagicNumber = MAGICNUMBER;
    newHash->m_HashFunc = _hashFunc;
    newHash->m_Equality = _keysEqualFunc;

return newHash;
}



void NOTHashMap_Destroy(HashMap** _map, void (*_keyDestroy)(void* _key), void (*_valDestroy)(void* _value))
{

    elementsRemover* removingfunction;
    ListItr begin,end;
    size_t i,numberoflists;
    List* list;

    if(NULL != _map && NULL != *_map && MAGICNUMBER == (*_map)->m_MagicNumber)
    {

        removingfunction = calloc(1,sizeof(elementsRemover));
        CHECK_EQUAL_VOID_RETURN(NULL,removingfunction);
        removingfunction->m_destroyKey = _keyDestroy;
        removingfunction->m_destroyValue = _valDestroy;
        numberoflists = Vector_Size((*_map)->m_vector);

        /*cant use vector for each couse give memory to NULL removing function vector start at 1*/
        for(i = 1; i<=numberoflists ; ++i)
        {
            Vector_Get((*_map)->m_vector,i,(void*)&list);

            begin = ListItr_Begin(list);
            end = ListItr_End(list);
            ListItr_ForEach(begin,end,(ListActionFunction)removItemFromList,removingfunction);
            List_Destroy(&list,NULL);
        }
        free(removingfunction);
        (*_map)->m_MagicNumber = 0;
        free(*_map);
        *_map= NULL;
    }

return;
}

Map_Result NOTHashMap_Insert(HashMap* _map, const void* _key, const void* _value)
{
    size_t index;
    List* list;
    ListItem* newitem;
    void* tempitr;
    searchandkey tempsearch;
    List_Result res;
    void* tempitem;

    CHECK_EQUAL_RETURN(NULL,_map,MAP_UNINITIALIZED_ERROR);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_map->m_MagicNumber,MAP_UNINITIALIZED_ERROR);
    CHECK_EQUAL_RETURN(NULL,_key,MAP_KEY_NULL_ERROR);
    CHECK_EQUAL_RETURN(NULL,_value,MAP_KEY_NULL_ERROR);

    index = getIndex(_map,_key);

    Vector_Get(_map->m_vector,index, (void**)&list);
    tempsearch.m_equality = _map->m_Equality;
    tempsearch.m_key =(void*) _key;

    if(searchduplic(list,&tempsearch,(void*)&tempitem,(void*)&tempitr))
    {
        return MAP_KEY_DUPLICATE_ERROR;
    }

    newitem = (ListItem*)malloc(sizeof(ListItem));
    CHECK_EQUAL_RETURN(NULL,newitem,MAP_ALLOCATION_ERROR);

    newitem->m_key = (void*)_key;
    newitem->m_value = (void*)_value;

    res = List_PushHead(list,newitem);


    if(LIST_SUCCESS != res)
    {
        free(newitem);
        return MAP_ALLOCATION_ERROR;
    }
    ++_map->m_NumberOfElements;

return MAP_SUCCESS;
}

Map_Result NOTHashMap_Find(const HashMap* _map, const void* _searchKey, void** _ppValue)
{
    List* list;
    int anser;
    ListItem* tempitem;
    ListItr* tempitr;
    size_t index;
    searchandkey tempsearch;

    CHECK_EQUAL_RETURN(NULL,_map,MAP_UNINITIALIZED_ERROR);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_map->m_MagicNumber,MAP_UNINITIALIZED_ERROR);
    CHECK_EQUAL_RETURN(NULL,_searchKey,MAP_KEY_NULL_ERROR);


    index = getIndex((HashMap*)_map,(void*)_searchKey);
    Vector_Get(_map->m_vector,index,(void*)&list);

    tempsearch.m_equality = _map->m_Equality;
    tempsearch.m_key =(void*) _searchKey;

    anser = searchduplic(list,&tempsearch,(void*)&tempitem,(void*)&tempitr);
    if(!anser)
    {
        return MAP_KEY_NOT_FOUND_ERROR;
    }

    *_ppValue=tempitem->m_value;

return MAP_SUCCESS;
}

Map_Result NOTHashMap_Remove(HashMap* _map, const void* _searchKey, void** _pKey, void** _pValue)
{
    List* list;
    size_t index;
    int anser;
    ListItem* itemToDestroy;
    ListItr* itrToRemove;
    searchandkey tempsearch;

    CHECK_EQUAL_RETURN(NULL,_map,MAP_UNINITIALIZED_ERROR);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_map->m_MagicNumber,MAP_UNINITIALIZED_ERROR);
    CHECK_EQUAL_RETURN(NULL,_pKey,MAP_KEY_NULL_ERROR);
    CHECK_EQUAL_RETURN(NULL,_pValue,MAP_KEY_NULL_ERROR);


    index =getIndex(_map,_searchKey);
    Vector_Get(_map->m_vector,index,(void*)&list);
    tempsearch.m_equality = _map->m_Equality;
    tempsearch.m_key =(void*) _searchKey;

    anser = searchduplic(list,&tempsearch,(void*)&itemToDestroy,(void*)&itrToRemove);
    if(!anser)
    {
        return MAP_KEY_NOT_FOUND_ERROR;
    }

    *_pKey = itemToDestroy->m_key;
    *_pValue = itemToDestroy->m_value;
    itemToDestroy = ListItr_Remove(itrToRemove);
    --_map->m_NumberOfElements;

return MAP_SUCCESS;
}

size_t NOTHashMap_Size(const HashMap* _map)
{
    CHECK_EQUAL_RETURN(NULL,_map,MAP_UNINITIALIZED_ERROR);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER ,_map->m_MagicNumber,MAP_UNINITIALIZED_ERROR);
    return _map->m_NumberOfElements;
}

Map_Result NOTHashMap_Rehash(HashMap *_map, size_t newCapacity)
{
    Vector_t* newVec;
    size_t oldSize;

    CHECK_EQUAL_RETURN(NULL,_map,MAP_UNINITIALIZED_ERROR);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER ,_map->m_MagicNumber,MAP_UNINITIALIZED_ERROR);

    if(newCapacity <= _map->m_Size)
    {
        return MAP_ALLOCATION_ERROR;/*rename for null map or iligal size*/
    }
    /*keep the old size for removing the old vec later*/
    oldSize = _map->m_Size;
    _map->m_Size = NextPrimeNumber( newCapacity);

    newVec = MoveItemsFromOldVectorToNewVector(_map,oldSize);
    CHECK_EQUAL_RETURN(NULL,newVec,MAP_ALLOCATION_ERROR);

    Vector_Destroy(&_map->m_vector,NULL);
    _map->m_vector = newVec;

return MAP_SUCCESS;
}

size_t NOTHashMap_ForEach(const HashMap* _map, KeyValueActionFunction _action, void* _context)
{
    size_t numberoflists,i;

    actionandfunction* action;
    ListItr begin,end;
    List* list;

    CHECK_EQUAL_RETURN(NULL,_map,FALSE);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER ,_map->m_MagicNumber,FALSE);
    CHECK_EQUAL_RETURN(NULL,_action,FALSE);

    action = malloc(sizeof(actionandfunction));
    CHECK_EQUAL_RETURN(NULL,action,FALSE);

    numberoflists = Vector_Size(_map->m_vector);
    action->action=_action;
    action->contex = _context;

    /*cant use vector for each couse give memory to NULL removing function*/
    for(i = 1; i<=numberoflists ; ++i)
    {
        Vector_Get(_map->m_vector,i,(void*)&list);

        begin = ListItr_Begin(list);
        end = ListItr_End(list);
        ListItr_ForEach(begin,end,(ListActionFunction)revVal,action);
    }
    free(action);
return TRUE;
}

#ifndef NDEBUG
Map_Stats* NOTHashMap_GetStatistics(const HashMap* _map)
{

    Map_Stats* newStatMap;
    size_t* numberOfChainLighnt;
    List* list;
    size_t i,maxChainNumber = 0;

    CHECK_EQUAL_RETURN(NULL,_map,NULL);
    CHECK_NOTEQUAL_RETURN(MAGICNUMBER ,_map->m_MagicNumber,NULL);

    newStatMap = malloc(sizeof(Map_Stats));
    CHECK_EQUAL_RETURN(NULL,newStatMap,NULL);

    numberOfChainLighnt = calloc(_map->m_Size,sizeof(int));
    CHECK_EQUAL_RETURN(NULL,numberOfChainLighnt,NULL);

    for(i = 1 ; i < _map->m_Size; ++i)
    {
        Vector_Get(_map->m_vector,i,(void*)&list);
        numberOfChainLighnt[i-1]= List_Size(list);
        if(numberOfChainLighnt[i-1] > maxChainNumber)
        {
            maxChainNumber = numberOfChainLighnt[i-1];
        }
    }

    newStatMap->numberOfBuckets = _map->m_Size;
    newStatMap->numberOfNonEmptyBuckets = _map->m_NumberOfElements;
    newStatMap->maxChainLength = maxChainNumber;
    newStatMap->averageChainLength = (maxChainNumber/_map->m_NumberOfElements);

return newStatMap;
}
#endif /* NDEBUG */

static int revVal(ListItem* _item, actionandfunction* _action)
{
    return ((KeyValueActionFunction)(_action->action)(_item->m_key,_item->m_value ,_action->contex));
}


static Vector_t* MoveItemsFromOldVectorToNewVector(HashMap* _map,size_t oldSize)
{
    size_t i,index;
    Vector_t* newVec;
    ListItem* tempItem;
    List* oldListToremoveFrom;
    List* newListToInsertTo;
    /*List_Result res;*/
    Vector_Result res;
    ListItr* begin;
    ListItr* end;

    newVec = Vector_Create( _map->m_Size,0);
    CHECK_EQUAL_RETURN(NULL,newVec,NULL);

    /*creaty new ve and new lists ---- dose malloc check neads??*/
    for( i = 1; i <= _map->m_Size ;++i)
    {
        newListToInsertTo = List_Create();
        res = Vector_Append(newVec,newListToInsertTo);
        CHECK_NOTEQUAL_RETURN(VECTOR_SUCCESS ,res,NULL);

    }

    for(i = 1;i <= oldSize; ++i)
    {
        Vector_Get(_map->m_vector,i, (void**)&oldListToremoveFrom);
        begin = ListItr_Begin(oldListToremoveFrom);
        end = ListItr_End(oldListToremoveFrom);
        while (begin != end)
        {
            tempItem = ListItr_Get(begin);

            index = getIndex(_map,tempItem->m_key);
            Vector_Get(newVec,index, (void**)&newListToInsertTo);/*get the list for new location for item*/
            res = List_PushHead(newListToInsertTo,tempItem); /* set the item in the new location*/

            begin = ListItr_Next(begin);
        }
        List_Destroy(&oldListToremoveFrom,NULL);
    }

return newVec;
}

static size_t getIndex(const HashMap* _map , const void* _searchKey)
{
    size_t index;

    index = _map->m_HashFunc(_searchKey);
    index = 1 + (index%_map->m_Size);

return index;

}
/* the function search duplicate items on the list*/
static int searchduplic(List* _list,searchandkey* _searchkey, void** _ppitem,void** _ppListItr)
{
    ListItr begin;
    ListItr end;
    begin = ListItr_Begin(_list);
    end = ListItr_End(_list);

    /*getting the item from the list and send to separed function */
    begin = ListItr_FindFirst(begin,end,(PredicateFunction)seperadeKeyFromItem,_searchkey);

    if(begin != end)
    {
        *_ppitem = ListItr_Get(begin);
        *_ppListItr=begin;
        return TRUE;
    }

return FALSE;
}

/* the function separed the key from struction and send to user equlty function*/
int seperadeKeyFromItem(ListItem* _item,searchandkey* _searchkey)
{
    return _searchkey->m_equality(_item->m_key,_searchkey->m_key);

}

static size_t NextPrimeNumber(size_t _size)
{
    while(!isPrime(_size))
    {
        ++_size;
    }
return  _size;
}

static size_t isPrime(size_t _number)
{
    size_t i;
    for(i=2; i < sqrt(_number);++i)
    {
        if( _number % i == 0)
        {
            return 0;
        }
    }
return 1;
}


/* removing items from list --- the function check for null pointers */
static void removItemFromList(ListItem* _item,elementsRemover* elementsRemover)
{

    if(NULL != *elementsRemover->m_destroyKey)
    {
        elementsRemover->m_destroyKey(_item->m_key);
    }

    if(NULL != *elementsRemover->m_destroyValue)
    {
       elementsRemover->m_destroyValue(_item->m_value);
    }

    free(_item);
}
