/*
 * client_ui.h
 *
 *  Created on: Jul 13, 2017
 *      Author: pavel
 */

#ifndef CLIENT_UI_H_
#define CLIENT_UI_H_

typedef struct Client_Ui Client_Ui;

/*Define Enum of Ui request*/
typedef enum UiRequst
{
    UI_SIGNUP,
    UI_LOGIN,
    UI_LOGOUT,
    UI_DELTEUSER,
    UI_NEW_GROUP,
    UI_JOIN_GRUP,
    UI_LEAVE_GROUP,
    UI_DESTROY_GROUP,
    UI_GET_ALL_GROUPS
}UiRequst;


/*Define Enum of Client Receive Status*/
typedef enum ClientRecieveStatus
{
    CLIENT_STATUS_OK,
    CLIENT_STATUS_BAD_PARAM,
    CLIENT_STATUS_NAME_TAKEN,
    CLIENT_STATUS_ALRDY_IN_GROUP,
    CLIENT_STATUS_USER_NOT_FOUND,
	CLIENT_STATUS_GROUP_DO_NOT_EXIST,
    CLIENT_STATUS_NO_FREE_IP,
    CLIENT_STATUS_SERVER_FALL
}ClientRecieveStatus;

/**
* \brief the function for response user inserts
* @param int _select - user selction
* @param char* _param1 - optional parum ;
* @param char* _param2 - optional parum ;
* @param void* _context - user provided context in run
* @return void
*/
typedef ClientRecieveStatus (*actionFunction)(int _selct,char* _param1, char* _param2,void* _contxt);

/**
* \brief the function for create new Client
* @param actionFunction _function - user action function for client selection
* @return in success Client_Ui* in fail NULL.
*/
Client_Ui* Ui_Create(actionFunction _function);

/**
* brief the function run Ui interface
* Client_Ui* _ui - Client_ui to run
* void* _context - context for action function.
*/
void Ui_Run(Client_Ui* _ui, void* _context);


/**
* brief the function Destroy Ui interface
* Client_Ui* _ui - Client_ui to destroy
*/
void Ui_Destroy(Client_Ui* _ui);

#endif /* CLIENT_UI_H_ */
