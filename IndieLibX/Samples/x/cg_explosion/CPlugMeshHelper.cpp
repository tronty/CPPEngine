
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

// OGL OGL OGL OGL OGL OGL
#include <windows.h>
#include <gl/gl.h>
#include <gl\glext.h>
// OGL OGL OGL OGL OGL OGL

#ifndef LPCSTR
#	define LPCSTR const char *
#endif

#include "nv_plug/CPlugMeshHelper.h"

PlugMeshHelper::PlugMeshHelper()
{
	//m_plugnumsrc->setImmediateFlush(false);
	m_plugMesh= plug_newComposite("mesh", PLUG_GLPLUGMESH, "MESH");
		m_plugVtx = m_plugMesh->plug_new("vtx", PLUG_GLPLUGMESHVTX, PLUG_FLOATVEC3);
		m_plugColor = m_plugMesh->plug_new("colors", PLUG_GLPLUGMESHCOLOR, PLUG_FLOATVEC4);
		m_plugNormal = m_plugMesh->plug_new("normals", PLUG_GLPLUGMESHNORMAL, PLUG_FLOATVEC3);
		m_plugUV[0] = m_plugMesh->plug_new("uvcoords0", PLUG_GLPLUGMESHUV0, PLUG_FLOATVEC3);
		m_plugUV[1] = m_plugMesh->plug_new("uvcoords1", PLUG_GLPLUGMESHUV1, PLUG_FLOATVEC3);
		m_plugUV[2] = m_plugMesh->plug_new("uvcoords2", PLUG_GLPLUGMESHUV2, PLUG_FLOATVEC3);
		m_plugUV[3] = m_plugMesh->plug_new("uvcoords3", PLUG_GLPLUGMESHUV3, PLUG_FLOATVEC3);
}

PlugMeshHelper::~PlugMeshHelper()
{
}
/*----------------------------------------------------------------------------------*/ /**

Called when some plugs of this container are going to be changed

**/ //----------------------------------------------------------------------------------
bool PlugMeshHelper::plug_IncomingData(IPlug *lpSrc, IPlug *lpDst, bool bSendEvent)
{
	if(lpSrc != lpDst)
	{
		int i=0;
		PlugType srctype = lpSrc->getType();
		PlugType dsttype = lpDst->getType();
		//
		// here is the composite type : must be of same type
		//
		if((srctype == PLUG_COMPOSITE)
		&&(dsttype == PLUG_COMPOSITE))
			return Incoming_Composite(lpSrc, lpDst, bSendEvent);
		//
		// Here are other plugs with various known id's
		//
		unsigned short id = lpDst->getId();
		int sz = lpDst->arraySize();
		void *ptr = lpDst->lpData();
		switch(id)
		{
		case PLUG_GLPLUGMESHVTX:
			if(srctype != PLUG_FLOATVEC3)
				return false;
			lpSrc->getValues(ptr, sz);
			break;
		case PLUG_GLPLUGMESHCOLOR:
			if(srctype != PLUG_FLOATVEC4)
				return false;
			lpSrc->getValues(ptr, sz);
			break;
		case PLUG_GLPLUGMESHNORMAL:
			if(srctype != PLUG_FLOATVEC3)
				return false;
			lpSrc->getValues(ptr, sz);
			break;
		case PLUG_GLPLUGMESHUV0:
		case PLUG_GLPLUGMESHUV1:
		case PLUG_GLPLUGMESHUV2:
		case PLUG_GLPLUGMESHUV3:
			if(srctype != PLUG_FLOATVEC3)
				return false;
			lpSrc->getValues(ptr, sz);
			break;
		}
	}
	return true;
}
/*----------------------------------------------------------------------------------*/ /**

Called when some plugs of this container are being read

**/ //----------------------------------------------------------------------------------
void *PlugMeshHelper::plug_DataReadEvent(IPlug *lpPlug, int &idx, int *arraysize)
{
	if(!lpPlug)
		return NULL;
	if(!_mesh)// No data attached here
	{
		if(arraysize) *arraysize = 0;
		return NULL;
	}
	int i=0;
	PlugType type = lpPlug->getType();
	unsigned short id = lpPlug->getId();
	switch(id)
	{
	case PLUG_GLPLUGMESHVTX:
		if(!_mesh->offs_vtx) // if this field is empty
		{
			if(arraysize) *arraysize = 0;
			return NULL;
		}
		// just point to the values
		if(arraysize) *arraysize = _mesh->num_vtx;
		return (void*)(_mesh->data + _mesh->offs_vtx + (sizeof(float)*3*idx));
		break;
	case PLUG_GLPLUGMESHCOLOR:
		if(!_mesh->offs_color) // if this field is empty
		{
			if(arraysize) *arraysize = 0;
			return NULL;
		}
		// just point to the values
		if(arraysize) *arraysize = _mesh->num_color;
		return (void*)(_mesh->data + _mesh->offs_color + (sizeof(float)*4*idx));
		break;
	case PLUG_GLPLUGMESHNORMAL:
		if(!_mesh->offs_normal) // if this field is empty
		{
			if(arraysize) *arraysize = 0;
			return NULL;
		}
		// just point to the values
		if(arraysize) *arraysize = _mesh->num_normal;
		return (void*)(_mesh->data + _mesh->offs_normal + (sizeof(float)*3*idx));
		break;
	case PLUG_GLPLUGMESHUV0:
	case PLUG_GLPLUGMESHUV1:
	case PLUG_GLPLUGMESHUV2:
	case PLUG_GLPLUGMESHUV3:
		i = id - PLUG_GLPLUGMESHUV0;
		if(!_mesh->offs_uv[i]) // if this field is empty
		{
			if(arraysize) *arraysize = 0;
			return NULL;
		}
		// just point to the values
		if(arraysize) *arraysize = _mesh->num_uv[i];
		return (void*)(_mesh->data + _mesh->offs_uv[i] + (sizeof(float)*3*idx));
		break;
	}
	return NULL;
}




