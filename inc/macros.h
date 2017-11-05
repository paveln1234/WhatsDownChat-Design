#ifndef MACROS_H_INCLUDED
#define MACROS_H_INCLUDED

#define CHECK_EQUAL_RETURN(check, checkAgainst,returnVal) if(checkAgainst == check) \
															{ \
															return returnVal; \
															}
#define CHECK_NOTEQUAL_RETURN(check, checkAgainst,returnVal) if(checkAgainst != check) \
															{ \
															return returnVal; \
															}
#define CHECK_EQUAL_VOID_RETURN(check, checkAgainst) if(checkAgainst == check) \
															{ \
															return ; \
															}

#define CHECK_NOTEQUAL_VOID_RETURN(check, checkAgainst) if(checkAgainst != check) \
															{ \
															return; \
															}


#endif /* MACROS_H_INCLUDED*/
