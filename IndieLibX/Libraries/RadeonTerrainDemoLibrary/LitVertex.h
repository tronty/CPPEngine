

//-----------------------------------------------------------------------------
// File: LitVertex.h
//
//  Terrain Demo.
//
// Copyright (c) 2000 ATI Technologies Inc. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef _LITVERTEX_H
#define _LITVERTEX_H

#include "DXErrors.h"
typedef unsigned int __D3DCOLOR__;

// Prelit vertex with one set of texture coordinates
#define D3DFVF_LVERTEX1 (D3DFVF_XYZ | D3DFVF_DIFFUSE |D3DFVF_TEX1)

typedef struct _LITVERTEX
{
    float x, y, z;
    __D3DCOLOR__ color;
    float tu, tv;
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
    _LITVERTEX() { }
	
    _LITVERTEX(const _LITVERTEX& rhs)
	{
		color=rhs.color;
		x=rhs.x;
		y=rhs.y;
		z=rhs.z;
		tu=rhs.tu;
		tv=rhs.tv;
	}
    _LITVERTEX(const D3DXFROMWINEVECTOR3& v, __D3DCOLOR__ _color, float _tu, float _tv)
    {
        x = v.x; y = v.y; z = v.z;
        color = _color; tu = _tu; tv = _tv;
    }
#endif
} LITVERTEX, *LPLITVERTEX;

typedef struct _LITVERTEX2
{
    float x, y, z;
    float tu, tv;
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
    _LITVERTEX2() { }
    _LITVERTEX2(const D3DXFROMWINEVECTOR3& v, float _tu, float _tv)
    {
        x = v.x; y = v.y; z = v.z; tu = _tu; tv = _tv;
    }
	
    _LITVERTEX2(const _LITVERTEX2& rhs)
	{
		x=rhs.x;
		y=rhs.y;
		z=rhs.z;
		tu=rhs.tu;
		tv=rhs.tv;
	}
    _LITVERTEX2(const _LITVERTEX& rhs)
	{
		x=rhs.x;
		y=rhs.y;
		z=rhs.z;
		tu=rhs.tu;
		tv=rhs.tv;
	}
#endif
} LITVERTEX2, *LPLITVERTEX2;

//#define D3DFVF_LVERTEX1 ZZZ
typedef struct _D3DVERTEX {
  union {
    float x;         /* Homogeneous coordinates */
    float dvX;
  };
  union {
    float y;
    float dvY;
  };
  union {
    float z;
    float dvZ;
  };
  union {
    float nx;        /* Normal */
    float dvNX;
  };
  union {
    float ny;
    float dvNY;
  };
  union {
    float nz;
    float dvNZ;
  };
  union {
    float tu;        /* Texture coordinates */
    float dvTU;
  };
  union {
    float tv;
    float dvTV;
  };
#if 1//(DIRECT3D_VERSION >= 0x0500)
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
  _D3DVERTEX() { }
  _D3DVERTEX(const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& v, 
const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& n, 
float _tu, float _tv)
    {
      x = v.x; y = v.y; z = v.z;
      nx = n.x; ny = n.y; nz = n.z;
      tu = _tu; tv = _tv;
    }
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
  enum _FVF
    {
        FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1
    };
} D3DVERTEX, *LPD3DVERTEX;
#define D3DFVF_VERTEX D3DFVF_XYZ|D3DFVF_TEX1




typedef struct _D3DVERTEX2 {
  union {
    float x;         /* Homogeneous coordinates */
    float dvX;
  };
  union {
    float y;
    float dvY;
  };
  union {
    float z;
    float dvZ;
  };/*
  union {
    float nx;        // Normal 
    float dvNX;
  };
  union {
    float ny;
    float dvNY;
  };
  union {
    float nz;
    float dvNZ;
  };*/
  union {
    float tu;        /* Texture coordinates */
    float dvTU;
  };
  union {
    float tv;
    float dvTV;
  };
#if 1//(DIRECT3D_VERSION >= 0x0500)
#if 1//(defined __cplusplus) && (defined D3D_OVERLOADS)
  _D3DVERTEX2() { }
  _D3DVERTEX2(const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& v, 
//const /*D3DXFROMWINEVECTOR3*/D3DXFROMWINEVECTOR3& n, 
float _tu, float _tv)
    {
      x = v.x; y = v.y; z = v.z;
//      nx = n.x; ny = n.y; nz = n.z;
      tu = _tu; tv = _tv;
    }
  _D3DVERTEX2(D3DVERTEX v)
{
      x = v.x; y = v.y; z = v.z;
      tu = v.tu; tv = v.tv;
}
#endif
#endif /* DIRECT3D_VERSION >= 0x0500 */
  enum _FVF
    {
        FVF = D3DFVF_XYZ|
//D3DFVF_NORMAL|
D3DFVF_TEX1
    };
} D3DVERTEX2, *LPD3DVERTEX2;
#define D3DFVF_VERTEX D3DFVF_XYZ|D3DFVF_TEX1
#endif


