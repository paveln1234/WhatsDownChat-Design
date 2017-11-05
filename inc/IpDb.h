#ifndef IPDB_H_INCLUDED
#define IPDB_H_INCLUDED
#include <stddef.h>  /* size_t */
typedef struct IpDb_t IpDb_t;

typedef struct Ip_t
{
	struct sockaddr_in m_Addr;
}Ip_t;

typedef void (*destroyIpFunction)(void* _item);

/** \brief the function create  data base
 * \param size_t _dbSize - maximum db size
 * \return in success IpDb_t* in fail NULL;
 * \warning maximum db size must to be bigger than 100
 */
IpDb_t* IpDb_Creare(size_t _dbSize);

/** \brief the function Remove item from dB
 * \param IpDb_t* _db - database to add to him.
 * \param Ip_t* _returnIp - returning IP.
 * \return in success return 1 if user not exceed return 0
 */
int IpDb_ReturnIp(IpDb_t* _db,Ip_t* _returnIp);

/** \brief the function receive free ip if no free ip return null
 * \param IpDb_t* _db - database to Get ip from
 * \return in success return Ip_t* if fail or no free ip return NULL
 */
Ip_t* IpDb_GetIp(IpDb_t* _db);

/** \brief the function destroy a given IpDb_t and free the memory
 * \param IpDb_t* _dbToDistroy - IpDb_t to destroy
 * \return in success return 1 if user not exceed return 0
 */
int IpDb_Destroy(IpDb_t* _dbToDistroy);


#endif // IPDB_H_INCLUDED
