/*
 * Server.h
 *
 *  Created on: Jul 7, 2017
 *      Author: Pavel Korolin
 */

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

/** @brief  the module receive server configuration from "config.ini" file
 *    @warning if user change the configuration file type unpredictable behaver promise
 */

typedef struct ConfigStuct
{
	char m_ip[16];
	size_t m_port;
	size_t m_maxNumberOfClient; /* optinal */
	size_t m_waitingListSize; /*optinal*/
	size_t m_maxUsers;
	size_t m_maxGroups;
}ConfigStuct;


/* defult config */
#define DEFULT_IP "127.0.0.1"
#define DEFULT_PORT 7254
#define DEFULT_MAX_CLIENT 1000
#define DEFULT_WATING_LIST 300
#define DEFULT_MAX_USERS 1000
#define DEFULT_MAX_GROUPS 300

 /**
  * @brief the function create ServerInit  to use by the server
  * @param const char* _filename - server configuration file
  * @return in success ConfigStuct* , in fail NULL
  * @warning in case bad file name default configuration return
  */
ConfigStuct* Config_Get(const char* _filename);


 /**
  * @brief the function destroy a given ConfigStuct* and release the memory
  * @param ConfigStuct* _configStuctToDestroy - pointer to ConfigStuct To Destroy
  * @return in success 1 in fail 0
  */
int Config_Destroy(ConfigStuct* _configStuctToDestroy);

#endif /* CONFIG_H_INCLUDED*/
