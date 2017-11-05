#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "macros.h"

#define MAGICNUMBER  0xBEEFCACE

struct Vector{

		size_t m_magicNum;
	 	size_t m_numOfElements;
	 	size_t m_size;
	 	size_t m_initialSize;
	 	size_t m_extensionBlockSize;
		void** m_array;
};
/*internal function*/
static void swap (void** x, void** y);

Vector_t* Vector_Create(size_t _initialSize, size_t _extensionBlockSize)
{
	Vector_t* ptrVec;
	void** array;

	if(0 == _initialSize  && 0 == _extensionBlockSize )
	{
		return NULL;
	}

	ptrVec= calloc(1,sizeof(Vector_t));
	CHECK_EQUAL_RETURN(NULL,ptrVec,NULL);

	array= calloc(_initialSize,sizeof(void*));
	if (NULL == array)
	{
		free(ptrVec);
		return NULL;
	}

	ptrVec->m_magicNum=MAGICNUMBER;
	ptrVec->m_numOfElements=0;
	ptrVec->m_size=_initialSize;
 	ptrVec->m_initialSize=_initialSize;
 	ptrVec->m_extensionBlockSize=_extensionBlockSize;
	ptrVec->m_array=array;

	return ptrVec;
}

void Vector_Destroy (Vector_t** _vec, DestroyAction _elementDestroy)
{
	void* element = NULL;
	size_t i = 0;

	if( NULL != _vec  &&  NULL != *_vec  &&  MAGICNUMBER == (*_vec)->m_magicNum )
	{

		if (NULL != (*_vec)->m_array)
		{
			if (NULL != _elementDestroy )
			{
				for (i=1; i <= (*_vec)->m_numOfElements  ;  ++i)
				{
					Vector_Get(*_vec,i, &element);
					if(element)
						_elementDestroy(element);
				}
			}
			free((*_vec)->m_array);
		}
		(*_vec)->m_magicNum = 0;
		free(*_vec);
		*_vec=NULL;
	}

return;
}

int Vector_ForEeach (const Vector_t* _vec,VectorElementAction _action, int _counter, void* _context)
{
	int  i;
	int res = 0;
	void* element;

	CHECK_EQUAL_RETURN(NULL,_vec,FALSE);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,0);
	CHECK_EQUAL_RETURN(NULL,_action,FALSE);

	if(!_counter)
		_counter = _vec->m_numOfElements;

	for (i=1; i <= _vec->m_numOfElements && 0 <_counter;  ++i)
	{
		Vector_Get(_vec,i, &element);
		res =_action(element, i, _context);
		if(0 > res) /*error hapen*/
			return _counter;
		if(0 < res)
		{
			-- _counter;
		}

	}

return i-1;
}


Vector_Result Vector_Set (Vector_t* _vec, size_t _index, void** _pValue)
{
	CHECK_EQUAL_RETURN(NULL,_vec,VECTOR_UNINITIALIZED_ERROR);
	CHECK_EQUAL_RETURN(NULL,_pValue,VECTOR_UNINITIALIZED_ERROR);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,VECTOR_UNINITIALIZED_ERROR);
	/* check validity of the index */
	CHECK_EQUAL_RETURN(0,_index,VECTOR_INDEX_OUT_OF_BOUNDS_ERROR);

	if(_vec->m_numOfElements < _index)
	{
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}

	/* set new Ptr , and "send back" the old one */
	swap(&(_vec->m_array)[_index-1],_pValue);

return VECTOR_SUCCESS;
}



Vector_Result Vector_Get (const Vector_t* _vec, size_t _index, void** _pValue)
{
	CHECK_EQUAL_RETURN(NULL,_vec,VECTOR_UNINITIALIZED_ERROR);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,VECTOR_UNINITIALIZED_ERROR);
	/* check validity of the index */
	CHECK_EQUAL_RETURN(0,_index,VECTOR_INDEX_OUT_OF_BOUNDS_ERROR);

	if(_vec->m_numOfElements < _index)
	{
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}

	*_pValue=(_vec->m_array)[_index-1];

	return VECTOR_SUCCESS;
}

size_t Vector_Size (Vector_t* _vec)
{
	CHECK_EQUAL_RETURN(NULL,_vec,0);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,0);

return _vec->m_numOfElements;
}

size_t Vector_Capacity (Vector_t* _vec)
{
	CHECK_EQUAL_RETURN(NULL,_vec,0);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,0);

return _vec->m_size;
}

Vector_Result Vector_Append (Vector_t* _vec, void* _item)
{
	void** arr= NULL;
	size_t nOfElements;
	size_t newSize;

	CHECK_EQUAL_RETURN(NULL,_vec,VECTOR_UNINITIALIZED_ERROR);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,VECTOR_UNINITIALIZED_ERROR);


	nOfElements = _vec->m_numOfElements;

	if(	nOfElements == _vec->m_size)
	{
		CHECK_EQUAL_RETURN(0,_vec->m_extensionBlockSize,VECTOR_OVERFLOW);
		arr=_vec->m_array;

		newSize=(_vec->m_size)+(_vec->m_extensionBlockSize);
		arr=(void**) realloc(arr,newSize*sizeof(void*));
		CHECK_EQUAL_RETURN(NULL,arr,VECTOR_ALLOCATION_ERROR);

		_vec->m_size=newSize;
		_vec->m_array=arr;
	}

	(_vec->m_array)[nOfElements]=_item;
	_vec->m_numOfElements++;

return VECTOR_SUCCESS;
}

Vector_Result Vector_Remove (Vector_t* _vec, void** _pValue)
{
	void** arr= NULL;
	size_t nOfElements;
	size_t newSize;

	CHECK_EQUAL_RETURN(NULL,_vec,VECTOR_UNINITIALIZED_ERROR);
	CHECK_EQUAL_RETURN(NULL,_pValue,VECTOR_UNINITIALIZED_ERROR);
	CHECK_NOTEQUAL_RETURN(MAGICNUMBER,_vec->m_magicNum,VECTOR_UNINITIALIZED_ERROR);

	arr=_vec->m_array;
	nOfElements=_vec->m_numOfElements;
	CHECK_EQUAL_RETURN(0,nOfElements,VECTOR_UNDERFLOW);

	*_pValue=arr[nOfElements-1];
	_vec->m_numOfElements--;

	/* Shrink vector if there are more than 2 empty blocks */
	if( ((_vec->m_size)-nOfElements) >= 2*(_vec->m_extensionBlockSize) )
	{
		newSize=(_vec->m_size)-(_vec->m_extensionBlockSize);
		if (newSize > _vec->m_initialSize)
		{
			arr=(void**) realloc(arr,newSize*sizeof(void*));
			CHECK_EQUAL_RETURN(NULL,arr,VECTOR_ALLOCATION_ERROR);

			_vec->m_size=newSize;
			_vec->m_array=arr;
		}
	}

return VECTOR_SUCCESS;
}

void Vector_Print (Vector_t*_vec,VectorElementAction _elementPrint)
{
	if( NULL != _vec && MAGICNUMBER == _vec->m_magicNum )
	{
		Vector_ForEeach(_vec,_elementPrint,0,NULL);
	}

return;
}


static void swap (void** x, void** y)
{
	void* temp;

	temp=*x;
	*x=*y;
	*y=temp;
}



