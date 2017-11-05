#ifndef SERVERLOGIC_H_INCLUDED
#define SERVERLOGIC_H_INCLUDED

#include "Protocol.h"

/*Define ServerLogic_t Struct */
typedef struct ServerLogic_t ServerLogic_t;

/** \brief the function create new server logic that holds users and groups and ip addresses
 * \param size_t _maxNumberOfUsers - maximum number of users that cane be registry in the system the actual number will be up to the closes prim number
 * \param size_t _maxNumberOfGroups - maximum number of Groups  that cane be create in the system the actual number will be up to the closes prim number
 * \return in success ServerLogic_t* in fail NULL;
 # \warning if _maxNumberOfUsers  <100 or _maxNumberOfGroups <100 return NULL
 */
ServerLogic_t* ServerLogic_Create( size_t _maxNumberOfUsers,size_t _maxNumberOfGroups);

/** \brief the function run server Logic
 * \param ServerLogic_t* _serverToRun - serverLogic to run
 * \return in success 1 in fail 0;
 */
int ServerLogic_Run(ServerLogic_t* _serverToRun);

/** \brief the function Stop server Logic
 * \param ServerLogic_t* _serverToStop - serverLogic to stop
 * \return in success 1 in fail 0;
 */
int ServerLogic_Stop(ServerLogic_t* _serverToStop);

/** \brief the function destroy server Logic
 * \param ServerLogic_t* _serverToDestroy - serverLogic to destroy
 * \return in success 1 in fail 0;
 */
int ServerLogic_Destroy(ServerLogic_t* _serverToDestroy);

#endif // SERVERLOGIC_H_INCLUDED
