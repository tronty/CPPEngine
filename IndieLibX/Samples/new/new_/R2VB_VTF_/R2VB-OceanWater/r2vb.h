//-----------------------------------------------------------------------------
// File: r2vb.h
// Copyright (c) 2006 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __R2VB_H__
#define __R2VB_H__

#include "../../Framework/Direct3D/atir2vb.h"

typedef void* R2VBHANDLE;
typedef R2VBHANDLE* PR2VBHANDLE;

// checks to see if the device supports render to vertex buffer
HRESULT r2vbCheckCap (LPDIRECT3D9 pd3d);

// initialization of render to vertex buffer
HRESULT r2vbInit (LPDIRECT3DDEVICE9 pdev, DWORD size, PR2VBHANDLE phr2vb);

// disables render to vertex buffer and releases r2vb data
HRESULT r2vbTerm (R2VBHANDLE	phr2vb);

// sets the stream source to a render to vertex buffer texture
HRESULT r2vbSetStreamTexSource(
        R2VBHANDLE              hr2vb,
        DWORD                   stream,
        DWORD                   sampler,
        IDirect3DTexture9*      ptex,
        DWORD                   offset,
        DWORD                   stride);

// disable render to vertex buffer for this stream
HRESULT r2vbDisTexSource(R2VBHANDLE hr2vb, DWORD stream);

#endif //__R2VB_H__