//==================================//

// Index.h                          //

//                                  //

// Drew Card                        //

// ATI Research                     //

// Software Engineer                //

// 3D Application Research Group    //

// dcard@ati.com                    //

//==================================//



#ifndef INDEX_H

#define INDEX_H



/*

#ifdef _DEBUG

#pragma comment (lib, "index_t_d.lib")

#else

#pragma comment (lib, "index_t.lib")

#endif

*/

//==========//

// includes //

//==========//



//include <stdlib.h>
#if 1
#include <Framework3/IRenderer.h>
#else
#include <FrameworkHLSLCg3/stdlib.h>
#endif
#include <memory.h>

#include <string.h>



//==============//

// return codes //

//==============//

#define INDEX_OK                0

#define INDEX_REALLOCATE        -1

#define INDEX_MEMORY_ERROR      -2

#define INDEX_OUT_OF_RANGE      -3

#define INDEX_NOT_FOUND         -4





//==========//

// typedefs //

//==========//

typedef int (*sortFunction)(const void*, const void*);



typedef struct IndexS

{

   int           count;

   int           allocCount;

   int          *indices;

   char         *data;

   int           dataSize;

   sortFunction  sortFunc;

} IndexT;



//=====================//

// function prototypes //

//=====================//

int   indexFind(IndexT* v, void* element, int* result);

void  indexFree(IndexT* v);

int   indexSortedInsert(IndexT* v, int value);

void  indexArray(IndexT* index, char* array, int size, int count, sortFunction sortFunc);

void  indexArrayCB(IndexT* index, char* array, int size, int count, sortFunction sortFunc, sortFunction callbackFunc);

void  indexSortArray(IndexT* index, char* array, int size, int count, sortFunction sortFunc);



#endif
