#ifndef TLV_H_INCLUDED
#define TLV_H_INCLUDED
#include "Protocol.h"

/** \brief the function take variadic function arguments and  puts it in the buffer
 *              the function return the size of the data that was write
 * \param void* _buffer - buffer for th data
 * \param const char* _format - data from user
 * \return total size of the buffer after data insert to him.
 */
size_t Tlv_Decode(void* _buffer,  const char* _format, ...);




#endif /* TLV_H_INCLUDED*/
