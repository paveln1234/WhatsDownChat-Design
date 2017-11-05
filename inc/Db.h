#ifndef DB_H_INCLUDED
#define DB_H_INCLUDED
#include <stddef.h>  /* size_t */
typedef struct Db_t Db_t;
/** @brief Define of destroy function for items in db
    *@param void* item - the item that saved in db
    *@param void* _context - context provided by user
 item recieve from db con*/
typedef void (*destroyFunction)(void* _item,void* _contex);


/** \brief the function create  data base
 * \param size_t _dbSize - maximum db size
 * \return in success Db_t* in fail NULL;
 * \warning maximum db size must to be bigger than 100
 */
Db_t* Db_Creare(size_t _dbSize);

/** \brief the function add item to the data base
 * \param Db* _db - database to add to him.
 * \param const char* _key - item key
 * \param void* _data - user data
 * \return in success 1 if user exceed return 0
 */
int Db_Add(Db_t* _db,const char* _key, void* _data);

/** \brief the function check if key saved in Db
 * \param Db* _db - database to add to him.
 * \param const char* _key - key to search
 * \return in success 1 if not fund return 0
 */
int Db_IsInDb(Db_t* _db,const char* _key);


/** \brief the function Remove item from dB
 * \param Db_t* _db - database to add to him.
 * \param const char* _key - item key to remove
  * \param destroyFunction _destroyKeyFunction : pointer to function to destroy key
  * \param destroyFunction _destroyItemFunction : pointer to function to destroy items
  * \param void* _context : user context for the destroy function
 * \return in success return 1,if user not exceed return 0 return -1 if more items have in the DB;
 */
int Db_Remove(Db_t* _db,const char* _key,destroyFunction _destroyKeyFunction,destroyFunction _destroyItemFunction,void* _context);

/** \brief the function receive data that storage in the dB by key
 * \param Db_t* _db - database to add to him.
 * \param const char* _key - key to search
 * \param void* _returnDataBuffer - pointer for the return data
 * \return in success return 1 if data not exceed return 0
 */
int Db_GetData(Db_t* _db,const char* _key, void** _returnDataBuffer );

/** \brief the function destroy a given Db_t and free the memory
 * \param Db_t* _dbToDistroy - Db_t to destroy
  * \param destroyFunction _destroyKeyFunction : pointer to function to destroy key
  * \param destroyFunction _destroyItemFunction : pointer to function to destroy items
 * \return in success return 1 if user not exceed return 0
 */
int Db_Destroy(Db_t* _dbToDistroy,destroyFunction _destroyKeyFunction,destroyFunction _destroyItemFunction );

/** \brief the function pur all items in to string whit a given demitted between the items
 * \param Db_t* _dbToDistroy - Db_t to destroy
 * \return in success return 1 if user not exceed return 0
 */
int Db_GetAllItems(Db_t* _db, void* _buffer, char* _demitted);

#endif // USERDB_H_INCLUDED
