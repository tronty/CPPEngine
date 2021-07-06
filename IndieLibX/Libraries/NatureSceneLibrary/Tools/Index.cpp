// ATI's code



#include "Index.h"



#define INIT_ALLOC 1000



int indexBinaryTraverse(void *value, void *data, int *index, int stride, int count, int *result, int (*compare)(const void *, const void *));

int indexAddElement(IndexT *index, int value, int position, char append);

int indexAppend(IndexT *index, int value, int position);

int indexAppendBack(IndexT *index, int value);

int indexCheckAlloc(IndexT *index);

int indexInsert(IndexT *index, int value, int position);

int indexRemovePosition(IndexT *index, int position);

int indexRemoveSortedElement(IndexT *index, void *element);

int indexSortedInsert(IndexT *ind, int value);

int indexSortedInsertDup(IndexT *ind, int value);

int indexFind(IndexT *index, void *element, int *result);



void indexArray(IndexT *index, char *array, int size, int count, sortFunction sortFunc)

{

	int i;



	stx_memset(index, 0, sizeof(IndexT));

	index->data     = array;

	index->dataSize = size;

	index->sortFunc = sortFunc;



	for(i=0;i<count;i++)

		indexSortedInsert(index, i);

}



void indexArrayCB(IndexT *index, char *array, int size, int count, sortFunction sortFunc, sortFunction callback)

{

	int i, position, compValue, result;



	stx_memset(index, 0, sizeof(IndexT));

	index->data=array;

	index->dataSize=size;

	index->sortFunc=sortFunc;



	for(i=0;i<count;i++)

	{

		position=indexBinaryTraverse((char *)index->data+(i*index->dataSize), index->data, index->indices, index->dataSize, index->count, &compValue, index->sortFunc);



		if(compValue==0)

			callback((char *)index->data+(index->indices[position]*index->dataSize), (char *)index->data+(i*index->dataSize));

		else

		{

			if(compValue<0)

				result=indexInsert(index, i, position);

			else

				result=indexAppend(index, i, position);

		}

	}

}



void indexSortArray(IndexT *index, char *array, int size, int count, sortFunction sortFunc)

{

	int i;



	stx_memset(index, 0, sizeof(IndexT));

	index->data=array;

	index->dataSize=size;

	index->sortFunc=sortFunc;



	for(i=0;i<count;i++)

		indexSortedInsertDup(index, i);

}





//=============================================================================

// adds the passed element to the vector and either inserts or appends the 

// element with respect to the specified index based on the boolean value passed

// calling this function directly could result in an unsorted vector

//=============================================================================                         

int indexAddElement(IndexT* index, int value, int position, char append)

{

   int result;



   result = indexCheckAlloc(index);

   if (result == INDEX_MEMORY_ERROR)

   {

      return result;

   }



   if (append)

   {

      result = indexAppend(index, value, position);

   }

   else

   {

      result = indexInsert(index, value, position);

   }

   

   return result;

}



//=============================================================================

// appends the passed element after the specified index

// calling this function directly could result in an unsorted vector 

//=============================================================================                         

int indexAppend(IndexT* index, int value, int position)

{

   int result;



   if ((position < 0) || ((position >= index->count) && (index->count != 0)))

   {

      return INDEX_OUT_OF_RANGE;

   }



   result = indexCheckAlloc(index);

   if (result == INDEX_MEMORY_ERROR)

   {

      return result;

   }



   if ((index->count != 0) && (position < (index->count - 1)))

   {

      //=====================================================//

      // shift down by one element from the index to the end //

      //=====================================================//

      memmove(&index->indices[position + 2], &index->indices[position+ 1], (index->count - position - 1) * sizeof(int));

   }



   //=========================//

   // copy the new element in //

   //=========================//

   index->indices[position + 1] = value;

  

   index->count++;



   return INDEX_OK;

}



//=============================================================================

// appends the passed element to the back of the vector

// calling this function directly could result in an unsorted vector 

//=============================================================================                         

int indexAppendBack(IndexT* index, int value)

{

   int result;



   result = indexCheckAlloc(index);

   if (result == INDEX_MEMORY_ERROR)

   {

      return result;

   }



   //=========================//

   // copy the new element in //

   //=========================//

   index->indices[index->count] = value;

  

   index->count++;



   return INDEX_OK;

}



//=============================================================================

// checks if memory is still available in the vector and allocates more if 

// necessary 

//=============================================================================

int indexCheckAlloc(IndexT* index)

{

   int*    buffer;

   int    allocSize = 0;



   if ((index->allocCount == 0) && (index->count == 0))

   {

      index->allocCount = INIT_ALLOC;

      index->indices = (int*)malloc(index->allocCount * sizeof(int));

      if (index->indices == 0)

      {

         return INDEX_MEMORY_ERROR;

      }

      stx_memset(index->indices, 0, index->allocCount * sizeof(int));

   }

   

   if (index->count >= index->allocCount)

   {

      //============================================================//

      // we will allocate twice as much memory as is currently used //

      //============================================================//

      allocSize = (index->count << 1);



      buffer = (int*)malloc(allocSize  * sizeof(int));

      if (buffer == 0)

      {

         return INDEX_MEMORY_ERROR;

      }



      stx_memset(buffer, 0, allocSize * sizeof(int));



      //==============================================================================//

      // if we had memory allocated before then copy the old data into the new buffer //

      //==============================================================================//

      if ((index->allocCount != 0) && (index->indices != 0))

      {

         stx_memcpy(buffer, index->indices, index->allocCount * sizeof(int));

      }



      //================================//

      // free the old data if necessary //

      //================================//

      if (index->indices != 0)

      {

         free(index->indices);

      }



      //===============================================//

      // set the data pointer to point to the new data //

      //===============================================//

      index->indices = buffer;



      //============================//

      // record the allocation size //

      //============================//

      index->allocCount = allocSize;



      return INDEX_REALLOCATE;

   }



   return INDEX_OK;

}



//=============================================================================

// frees all resources used by the vector

//=============================================================================

void indexFree(IndexT* index)

{

   if (index->indices != 0)

   {

      free(index->indices);

   }



   stx_memset(index, 0, sizeof(IndexT));

}



//=============================================================================

// inserts the passed element into the vector before the index specified

// calling this function directly could result in an unsorted vector

//=============================================================================                         

int indexInsert(IndexT* index, int value, int position)

{

   int result;



   if ((position < 0) || ((position >= index->count) && (index->count != 0)))

   {

      return INDEX_OUT_OF_RANGE;

   }



   result = indexCheckAlloc(index);

   if (result == INDEX_MEMORY_ERROR)

   {

      return result;

   }



   //=====================================================//

   // shift down by one element from the index to the end //

   //=====================================================//

   if (index->count != 0) 

   {

      memmove(&index->indices[position + 1], &index->indices[position], (index->count - position) * sizeof(int));

   }



   //=========================//

   // copy the new element in //

   //=========================//

   index->indices[position] = value; 



   index->count++;



   return INDEX_OK;

}





//=============================================================================

// removes the element at the specified index

//=============================================================================                         

int indexRemovePosition(IndexT* index, int position)

{

   int result = INDEX_NOT_FOUND;



   if ((position < 0) || (position >= index->count))

   {

      return INDEX_OUT_OF_RANGE;

   }



   //=====================================================//

   // shift back by one element from the index to the end //

   //=====================================================//

   if (position < (index->count - 1)) 

   {

      memmove(&index->indices[position], &index->indices[position + 1], (index->count - position - 1) * sizeof(int));

   }



   index->count--;



   return INDEX_OK;

}



//=============================================================================

// finds the specified element and removes it

// INDEX MUST BE SORTED

//=============================================================================                         

int indexRemoveSortedElement(IndexT* index, void* element)

{

   int position;

   int value;

   int result = INDEX_NOT_FOUND;

   

   //=========================================================================//

   // Traverse the list of sorted data to find the index where we will insert //

   //=========================================================================//

   position = indexBinaryTraverse(

               element, index->data, index->indices, index->dataSize, index->count, &value, index->sortFunc);

   

   if (value == 0)

   {

      result = indexRemovePosition(index, position);

   }



   return result;

}



//=============================================================================

// inserts the passed element into the vector such that the vector is sorted

// INDEX MUST BE SORTED

//=============================================================================                         

int indexSortedInsert(IndexT* ind, int value)

{

   int position;

   int compValue;

   int result;

   

   //=========================================================================//

   // Traverse the list of sorted data to find the index where we will insert //

   //=========================================================================//

   position = indexBinaryTraverse(

               (char*)ind->data + (value * ind->dataSize), ind->data, ind->indices, ind->dataSize, ind->count, &compValue, ind->sortFunc);

   

   if (compValue == 0)

   {

      result = INDEX_OK;

   }

   else if (compValue < 0) // we are inserting before this index

   {

      result = indexInsert(ind, value, position);

   }

   else // we are appending after this index

   {

      result = indexAppend(ind, value, position);

   }



   return result;

}



//=============================================================================

// inserts the passed element into the vector such that the vector is sorted

// INDEX MUST BE SORTED

//=============================================================================                         

int indexSortedInsertDup(IndexT* ind, int value)

{

   int position;

   int compValue;

   int result;

   

   //=========================================================================//

   // Traverse the list of sorted data to find the index where we will insert //

   //=========================================================================//

   position = indexBinaryTraverse(

               (char*)ind->data + (value * ind->dataSize), ind->data, ind->indices, ind->dataSize, ind->count, &compValue, ind->sortFunc);

   

   if (compValue <= 0) // we are inserting before this index

   {

      result = indexInsert(ind, value, position);

   }

   else // we are appending after this index

   {

      result = indexAppend(ind, value, position);

   }



   return result;

}



//=============================================================================

// finds the specified element in the vector and returns its index

// INDEX MUST BE SORTED

//=============================================================================                         

int indexFind(IndexT* index, void* element, int* result)

{

   int position;

   

   //======================================================//

   // Traverse the list of sorted data to find the element //

   //======================================================//

   position = indexBinaryTraverse(

               element, index->data, index->indices, index->dataSize, index->count, result, index->sortFunc);

   

   return position;

}



//=============================================================================

// generic binary traversal function

//=============================================================================

int indexBinaryTraverse(

                   void* value,                             // pointer to the reference element   

                   void* data,                              // pointer to the indexed data      

                   int* indices,                            // pointer index      

                   int stride,                              // data stride      

                   int count,                               // number of items in the array      

                   int* result,                             // buffer for the result of the last compare

                   int (*compare)(const void*, const void*) // pointer to the compare function

                   )

{

   #pragma warning(push, 2)

   #pragma warning(disable:4311)

   #pragma warning(disable:4312)



   int high;

   int low;

   int mid;

   int nextMid;

   

   high = count;

   low = 0;

   mid = low + ((high - low) >> 1);

   *result = -1;



   while (low != high) 

   {

      *result = compare(value, (void*)((__DWORD__)data + indices[mid] * stride)); 

      if (*result == 0)

      {

         break;

      }

      else if (*result < 0)

      {

         high = mid;

      }

      else

      {

         low = mid;

      }

     

      nextMid = low + ((high - low) >> 1);

      if (mid == nextMid)

      {

         break;

      }



      mid = nextMid;

   }

   #pragma warning(pop)



   return mid;

}

