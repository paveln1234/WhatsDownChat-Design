#ifndef CLIENTLOGIC_H_INCLUDED
#define CLIENTLOGIC_H_INCLUDED


#include "Protocol.h"
/*Define ClientLogic_t Struct */
typedef struct ClientLogic_t ClientLogic_t;

/** \brief the function create new client logic.
 * \param const char* _serverIP - the server ip adress in ip4 format "XXX.XXX.XXX.XXX"
 * \param size_t _port - server port (port must be hier number than 1024);
 * \return in success ClientLogic_t* in fail NULL;
 */
ClientLogic_t* clientLogic_Create(const char* _serverIp,size_t _port);

/** \brief the function run client Logic
 * \param ClientLogic_t* _clientToRun - clientLogic to run
 * \return in success 1 in fail 0;
 */
int clientLogic_Run(ClientLogic_t* _clientToRun);

/** \brief the function Stop client Logic
 * \param ClientLogic_t* _clientToStop - clientLogic to stop
 * \return in success 1 in fail 0;
 */
int clientLogic_Stop(ClientLogic_t* _clientToStop);

/** \brief the function destroy client Logic
 * \param ClientLogic_t* _clientToDestroy - clientLogic to destroy
 * \return in success 1 in fail 0;
 */
int clientLogic_Destroy(ClientLogic_t* _clientToDestroy);

#endif /* CLIENTLOGIC_H_INCLUDED*/
