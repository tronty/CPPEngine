

//-----------------------------------------------------------------------------
// File: RenderQueue.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _RENDERQUEUE_H
#define _RENDERQUEUE_H

#include "DXErrors.h"
#include "MManager.h"
#include "TriangleList.h"

#define MAX_QUEUES MAX_MATERIALS
#define MAX_VB_QUEUES 10
// Maximum number of triangle lists in a single queue
#define MAX_QUEUE_SIZE 1000

typedef struct tagQUEUE
{
	VertexBufferID dwVBID;
	CTriangleList **pLists;
	__DWORD__ dwListCount;
} QUEUE;

class CRenderQueue
{
	QUEUE m_Queues[MAX_MATERIALS][MAX_VB_QUEUES];
	__DWORD__ m_dwVBQueueCount[MAX_MATERIALS];

public:
    CRenderQueue();
    ~CRenderQueue();
    int Init();
    void Destroy();
    void ClearQueue();
    int AddToQueue(CTriangleList *tlist);
    int Render( CVertexStore *pVS);
    __DWORD__ CountPolys();
};

#endif

