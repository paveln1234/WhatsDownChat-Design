/*
 * Client_ui.c
 *
 *  Created on: Jul 13, 2017
 *      Author: pavel
 */

#include <stdlib.h>
#include <stdio.h>
#include  <string.h>
#include "Client_ui.h"
#include "Client.h"
#define MAX_GROUP_NAME 32
#define FIFO_PATH "../msg"
struct Client_Ui
{
	actionFunction m_actionfunction;
};

/*********************************************/
static int MenuBeforLogin();
static int MenuAfterLogin();
static ClientRecieveStatus SignupFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus LoginFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus LogoutFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus DisconnectFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus DeleteUserFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus CreateNewGroupFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus JoinGruopFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus LeaveGroupFunction(Client_Ui* _ui, void* _context);
static ClientRecieveStatus GetAllGruopFunction(Client_Ui* _ui, void* _context);
static void PrintServerAnswer(ClientRecieveStatus _staus);
/************************************************/
Client_Ui* Ui_Create(actionFunction _function)
{

	Client_Ui* newClient = NULL;
    if(NULL == _function )
    {
		return NULL;
    }

	newClient = calloc(1,sizeof(Client_Ui));
    if(NULL == newClient)
        return NULL;

    newClient->m_actionfunction = _function;

return newClient;
}

void Ui_Run(Client_Ui* _ui, void* _context)
{

	int res =0;
	ClientRecieveStatus status;
	system("clear");
	printf("Program start \n ");
	printf("The program is initialized \n");

	do{
		res = MenuBeforLogin();
		switch (res)
		{
	        case 0:
	                printf("Exiting The Program\n");
	                break;
	        case 1:
	        	status =	SignupFunction(_ui,_context);
	                break;
	        case 2:
				status = 	LoginFunction(_ui,_context);
					break;
		}
		if(CLIENT_STATUS_OK != status)
			printf("\n name taken or bad paramters insert\n");
	}while(CLIENT_STATUS_OK != status && res);

	while(res)
	{
		res = MenuAfterLogin();
		switch (res)
		{
	        case 0:
	                {
	                	printf("Exiting The Program\n");
						break;
	                }
	        case 1:
	        		{
						status = LogoutFunction(_ui,_context);
						break;
	        		}
	        case 2:
					{
 						status = DisconnectFunction(_ui,_context);
						break;
					}
	        case 3:
					{
						status = DeleteUserFunction(_ui,_context);
						break;
					}

	        case 4:
					{
						status = CreateNewGroupFunction(_ui,_context);
						break;
					}

	        case 5:
					{
						status = JoinGruopFunction(_ui,_context);
						break;
					}

	        case 6:
	        		{
	        			status = LeaveGroupFunction(_ui,_context);
						break;
	        		}
	        case 7:
					{
						status = GetAllGruopFunction(_ui,_context);
						break;
					}
	        default:
	                    printf("Wrong Selection Please Select Again\n");
	                    system("clear");
	                    break;
		}
		PrintServerAnswer(status);

	}
}

void Ui_Destroy(Client_Ui* _ui)
{
    free(_ui);
}

static int MenuBeforLogin()
{
	size_t sel ;
	printf("\n Please select option \n");
	printf("\n Sign up to server --------------------> 1  \n");
	printf("\n Login to server ----------------------> 2  \n");
	printf("\n To Exit Press-------------------------> 0  \n");
	printf("\n Enter Selection : ");
	scanf("%d",&sel);
	system("clear");

return sel;
}
static int MenuAfterLogin()
{
	size_t sel ;
	printf("\n Please select option \n");
	printf("\n Log out from server ------------------> 1  \n");
	printf("\n Disconnect from server ---------------> 2  \n");
	printf("\n Delete user --------------------------> 3  \n");
	printf("\n Create new group ---------------------> 4  \n");
	printf("\n Join to group ------------------------> 5  \n");
	printf("\n Leave group --------------------------> 6  \n");
	printf("\n Get all groups -----------------------> 7  \n");
	printf("\n To Exit Press-------------------------> 0  \n");
	printf("\n Enter Selection : ");
	scanf("%d",&sel);
	system("clear");

return sel;


}

static ClientRecieveStatus SignupFunction(Client_Ui* _ui, void* _context)
{
	char userName[USERNAME_LENGTH]= {0};
	char password[PASSWORD_LENGTH]={0};
	system("clear");
	do
	{
		printf("Welcome to sing up to server screen \n ");
		printf("please insert user name \n");
		scanf("%s",userName);
		printf("please insert password \n");
		scanf("%s",password);
		if(!strlen(userName) || !strlen(password))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(userName) || !strlen(password));

return _ui->m_actionfunction(UI_SIGNUP,userName,password,_context);

}

/*******************************************/

static ClientRecieveStatus LoginFunction(Client_Ui* _ui, void* _context)
{
  	char userName[USERNAME_LENGTH]= {0};
	char password[PASSWORD_LENGTH]={0};
	system("clear");
	do
	{
		printf("Welcome to Login screen \n ");
		printf("please insert user name \n");
		scanf("%s",userName);
		printf("please insert password \n");
		scanf("%s",password);
		if(!strlen(userName) || !strlen(password))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(userName) || !strlen(password));

return _ui->m_actionfunction(UI_LOGIN,userName,password,_context);
}

static ClientRecieveStatus LogoutFunction(Client_Ui* _ui, void* _context)
{
    return _ui->m_actionfunction(UI_LOGOUT,NULL,NULL,_context);

}

static ClientRecieveStatus DisconnectFunction(Client_Ui* _ui, void* _context)
{
    return _ui->m_actionfunction(UI_LOGOUT,NULL,NULL,_context);
}

static ClientRecieveStatus DeleteUserFunction(Client_Ui* _ui, void* _context)
{
	char userName[USERNAME_LENGTH]= {0};
	char password[PASSWORD_LENGTH]={0};
	system("clear");
	do
	{
		printf("Welcome to delete user screen \n ");
		printf("please insert user name \n");
		scanf("%s",userName);
		if(!strlen(userName) || !strlen(password))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(userName) || !strlen(password));

return _ui->m_actionfunction(UI_DELTEUSER,userName,NULL,_context);

}

static ClientRecieveStatus CreateNewGroupFunction(Client_Ui* _ui, void* _context)
{
	ClientRecieveStatus status;
	char newGroup[MAX_GROUP_NAME]= {0};

	system("clear");
	do
	{
		printf("Welcome to create new group   screen \n ");
		printf("please insert group name \n");
        fgetc(stdin);
		fgets(newGroup,MAX_GROUP_NAME,stdin);

		if(!strlen(newGroup))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(newGroup));
    newGroup[strcspn(newGroup, "\n")] = 0;

    status = _ui->m_actionfunction(UI_NEW_GROUP,newGroup,NULL,_context);
    if(CLIENT_STATUS_OK == status)
	{
		return 1;
	}

return 0;
}

static ClientRecieveStatus JoinGruopFunction(Client_Ui* _ui, void* _context)
{
	char groupName[MAX_GROUP_NAME]= {0};

	system("clear");
	do
	{
		printf("Welcome to join to group   screen \n ");
		printf("please insert group name \n");
		fgetc(stdin);
		fgets(groupName,MAX_GROUP_NAME,stdin);
		if(!strlen(groupName))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(groupName));

    groupName[strcspn(groupName, "\n")] = 0;

return  _ui->m_actionfunction(UI_JOIN_GRUP,groupName,NULL,_context);

}

static ClientRecieveStatus LeaveGroupFunction(Client_Ui* _ui, void* _context)
{
	ClientRecieveStatus status;
	char groupName[32]= {0};

	system("clear");
	do
	{
		printf("Welcome to Leave group  screen \n ");
		printf("please insert group name \n");
		scanf("%s",groupName);
		if(!strlen(groupName))
		{
			printf("one of the arguments wrong \n");
		}
	}while(!strlen(groupName));

    status = _ui->m_actionfunction(UI_LEAVE_GROUP,groupName,NULL,_context);
    if(CLIENT_STATUS_OK == status)
	{
		return 1;
	}

return 0;
}

static ClientRecieveStatus GetAllGruopFunction(Client_Ui* _ui, void* _context)
{
	ClientRecieveStatus status;
	char buffer[1024]= {0};

	system("clear");
    status = _ui->m_actionfunction(UI_GET_ALL_GROUPS,buffer,NULL,_context);
	printf("Welcome to All group  screen \n ");
	printf("all the groups that available \n");
	printf("--------------------- \n");

	if(CLIENT_STATUS_OK == status)
	{
		printf("%s\n",buffer);
		printf("Press any key to continue \n");
		fgetc(stdin);

		fgetc(stdin);
	}

return status;
}
/**************************************************/

static void PrintServerAnswer(ClientRecieveStatus _staus)
{
	switch(_staus)
	{
		case CLIENT_STATUS_OK:
													{
														break;
													}
		case CLIENT_STATUS_BAD_PARAM:
												{
														printf("\n\n\t ONE OF THE PARAMETARS THAT INSERT WRONG");
														break;
												}
		case CLIENT_STATUS_NAME_TAKEN:
												{
														printf("\n\n\t NAME TAKEN");
														break;
												}
		case CLIENT_STATUS_ALRDY_IN_GROUP:
												{
														printf("\n\n\t ALRADY IN GROUP PLEASE LOGOUT FIRST");
														break;
												}
		case CLIENT_STATUS_USER_NOT_FOUND:
												{
														printf("\n\n\t NO USER WHIT THIS NAME , PLEASE SINGUP");
														break;
												}
		case CLIENT_STATUS_GROUP_DO_NOT_EXIST:
												{
														printf("\n\n\t GROUP NOT EXCIST, TRY ANOTHER NAME");
														break;
												}
		case CLIENT_STATUS_NO_FREE_IP:
												{
														printf("\n\n\t SERVER CANT OPEN NEW GROUP PLESE TRY LATER");
														break;
												}
		case CLIENT_STATUS_SERVER_FALL:
												{
														printf("\n\n\t OPPS... IT'S INBERASED SERVER FALL PLESE CONNECT LATER");
														break;
												}
	}


}
