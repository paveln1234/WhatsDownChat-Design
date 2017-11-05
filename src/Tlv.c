#include <stdio.h>/*sprintf*/
#include <stdlib.h> /*calloc */
#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include "Tlv.h"

size_t Tlv_Decode(void* _buffer,  const char* _format, ...)
{
    char* buffer = (char*)_buffer;
    char temp[1024];
    va_list msgAp;

    va_start(msgAp, _format);
        vsprintf(temp, _format, msgAp);
    va_end(msgAp);
return sprintf(buffer,"%s",temp);
}

