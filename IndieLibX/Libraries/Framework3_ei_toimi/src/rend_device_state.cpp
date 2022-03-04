//==================================================================
//  Copyright (C) 2006-2007  Davide Pasca
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//==================================================================
/// @file
///
///
///
/// @author Davide Pasca
///
//==================================================================
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#include <STX/STX.h>
#include <STX/STXCPP.h>
#include <Framework3/IRenderer.h>
#include <Framework3/rend_device_state.h>
using namespace PSYSGFX;
//==================================================================
///
//==================================================================

#ifdef __Framework3D3D9_H__
static unsigned int _locmap_TestFunc[] =
{
	D3DCMP_NEVER,		// TESTFUNC_NEVER,
	D3DCMP_LESS,		// TESTFUNC_LESS,
	D3DCMP_EQUAL,		// TESTFUNC_EQUAL,
	D3DCMP_LESSEQUAL,	// TESTFUNC_LESSEQUAL,
	D3DCMP_GREATER,		// TESTFUNC_GREATER,
	D3DCMP_NOTEQUAL,	// TESTFUNC_NOTEQUAL,
	D3DCMP_GREATEREQUAL,// TESTFUNC_GREATEREQUAL,
	D3DCMP_ALWAYS,		// TESTFUNC_ALWAYS,
};
#elif defined(__RendererGL_1_1_H__)
static unsigned int _locmap_TestFunc[] =
{
	GL_NEVER,		// TESTFUNC_NEVER,
	GL_LESS,		// TESTFUNC_LESS,
	GL_EQUAL,		// TESTFUNC_EQUAL,
	GL_LEQUAL,		// TESTFUNC_LESSEQUAL,
	GL_GREATER,		// TESTFUNC_GREATER,
	GL_NOTEQUAL,	// TESTFUNC_NOTEQUAL,
	GL_GEQUAL,		// TESTFUNC_GREATEREQUAL,
	GL_ALWAYS,		// TESTFUNC_ALWAYS,
};
#endif

#ifdef __Framework3D3D9_H__
static unsigned int _locmap_BlendFactor[] =
{
	D3DBLEND_ZERO,				// BLENDFCT_ZERO,
	D3DBLEND_ONE,				// BLENDFCT_ONE,
	D3DBLEND_SRCCOLOR,			// BLENDFCT_SRC_COLOR,
	D3DBLEND_INVSRCCOLOR,		// BLENDFCT_ONE_MINUS_SRC_COLOR,
	D3DBLEND_SRCALPHA,			// BLENDFCT_SRC_ALPHA,
	D3DBLEND_INVSRCALPHA,		// BLENDFCT_ONE_MINUS_SRC_ALPHA,
	D3DBLEND_DESTALPHA,			// BLENDFCT_DST_ALPHA,
	D3DBLEND_INVDESTALPHA,		// BLENDFCT_ONE_MINUS_DST_ALPHA,
	D3DBLEND_DESTCOLOR,			// BLENDFCT_DST_COLOR,
	D3DBLEND_INVDESTCOLOR,		// BLENDFCT_ONE_MINUS_DST_COLOR,
	D3DBLEND_SRCALPHASAT		// BLENDFCT_SRC_ALPHA_SATURATE
};
#elif defined(__RendererGL_1_1_H__)
static unsigned int	_locmap_BlendFactor[] =
{
	GL_ZERO,					// BLENDFCT_ZERO,
	GL_ONE,						// BLENDFCT_ONE,
	GL_SRC_COLOR,				// BLENDFCT_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,		// BLENDFCT_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,				// BLENDFCT_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,		// BLENDFCT_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,				// BLENDFCT_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,		// BLENDFCT_ONE_MINUS_DST_ALPHA,
	GL_DST_COLOR,				// BLENDFCT_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,		// BLENDFCT_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA_SATURATE		// BLENDFCT_SRC_ALPHA_SATURATE
};
#endif

//==================================================================
///
//==================================================================
void BlendingAlphaTest/*RendDeviceState*/::Reset()
{
	_blend = false;
    _blend_src = PSYSGFX::BLENDFCT_SRC_COLOR;//BLENDFCT_SRC_ALPHA;
    _blend_dst = PSYSGFX::BLENDFCT_DST_COLOR;//BLENDFCT_ONE_MINUS_SRC_ALPHA;

	_alphatst = false;
    _alphatst_func = TESTFUNC_GREATEREQUAL;//PSYSGFX::TESTFUNC_ALWAYS;
    _alphatst_ref = 0.8f/255.0f;//0.0f;
#ifdef __Framework3D3D9_H__
	LPDIRECT3DDEVICE9 d3ddevp=dynamic_cast<RendererHLSLD3D9>(IRenderer)->m_pd3dDevice;
	d3ddevp->SetRenderState( D3DRS_ALPHABLENDENABLE, _blend );
	d3ddevp->SetRenderState( D3DRS_SRCBLEND,	_locmap_BlendFactor[ _blend_src ] );
	d3ddevp->SetRenderState( D3DRS_DESTBLEND,	_locmap_BlendFactor[ _blend_dst ] );

	d3ddevp->SetRenderState( D3DRS_ALPHATESTENABLE, _alphatst );
	d3ddevp->SetRenderState( D3DRS_ALPHAFUNC, _locmap_TestFunc[_alphatst_func] );
	d3ddevp->SetRenderState( D3DRS_ALPHAREF, (int)(255*_alphatst_ref) );
#elif defined(__RendererGL_1_1_H__)
	if ( _blend )	glEnable( GL_BLEND );
	else			glDisable( GL_BLEND );

	if ( _alphatst )glEnable( GL_ALPHA_TEST );
	else			glDisable( GL_ALPHA_TEST );
	glAlphaFunc( _locmap_TestFunc[_alphatst_func], _alphatst_ref );
#endif
}

//==================================================================
void BlendingAlphaTest/*RendDeviceState*/::SetBlending( bool onoff )
{
	if ( _blend == onoff )
		return;
	_blend = onoff;

#ifdef __Framework3D3D9_H__
	LPDIRECT3DDEVICE9 d3ddevp=dynamic_cast<RendererHLSLD3D9>(IRenderer)->m_pd3dDevice;
	d3ddevp->SetRenderState( D3DRS_ALPHABLENDENABLE, _blend );
#elif defined(__RendererGL_1_1_H__)
	if ( _blend )	glEnable( GL_BLEND );
	else			glDisable( GL_BLEND );
#endif
}

//==================================================================
void BlendingAlphaTest/*RendDeviceState*/::SetBlending( bool onoff, unsigned int src, unsigned int dst )
{
	if ( _blend == onoff && !(onoff && (_blend_src != src || _blend_dst != dst) ) )
		return;
	_blend = onoff;

#ifdef __Framework3D3D9_H__
	LPDIRECT3DDEVICE9 d3ddevp=dynamic_cast<RendererHLSLD3D9>(IRenderer)->m_pd3dDevice;
	d3ddevp->SetRenderState( D3DRS_ALPHABLENDENABLE, _blend );
#elif defined(__RendererGL_1_1_H__)
	if ( _blend )	glEnable( GL_BLEND );
	else			glDisable( GL_BLEND );
#endif

	if ( _blend )
	{
		_blend_src = src;
		_blend_dst = dst;

	#ifdef __Framework3D3D9_H__
		d3ddevp->SetRenderState( D3DRS_SRCBLEND,	_locmap_BlendFactor[ _blend_src ] );
		d3ddevp->SetRenderState( D3DRS_DESTBLEND,	_locmap_BlendFactor[ _blend_dst ] );
	#elif defined(__RendererGL_1_1_H__)
		glBlendFunc( _locmap_BlendFactor[ _blend_src ], _locmap_BlendFactor[ _blend_dst ] );
	#endif
	}
}

//==================================================================
void BlendingAlphaTest/*RendDeviceState*/::SetAlphaTest( bool onoff, unsigned int tstfunc, float refparam )
{
	if ( _alphatst == onoff && _alphatst_func == tstfunc && _alphatst_ref == refparam )
		return;
	_alphatst = onoff;
	_alphatst_func = tstfunc;
	_alphatst_ref = refparam;

#ifdef __Framework3D3D9_H__
	LPDIRECT3DDEVICE9 d3ddevp=dynamic_cast<RendererHLSLD3D9>(IRenderer)->m_pd3dDevice;
	d3ddevp->SetRenderState( D3DRS_ALPHATESTENABLE, _alphatst );
	d3ddevp->SetRenderState( D3DRS_ALPHAFUNC, _locmap_TestFunc[_alphatst_func] );
	d3ddevp->SetRenderState( D3DRS_ALPHAREF, (int)(255*_alphatst_ref) );
#elif defined(__RendererGL_1_1_H__)
	if ( _alphatst )glEnable( GL_ALPHA_TEST );
	else			glDisable( GL_ALPHA_TEST );
	glAlphaFunc( _locmap_TestFunc[_alphatst_func], _alphatst_ref );
#endif
}

//==================================================================
void BlendingAlphaTest/*RendDeviceState*/::SetAlphaTest( bool onoff )
{
	if ( _alphatst == onoff )
		return;
	_alphatst = onoff;

#ifdef __Framework3D3D9_H__
	LPDIRECT3DDEVICE9 d3ddevp=dynamic_cast<RendererHLSLD3D9>(IRenderer)->m_pd3dDevice;
	d3ddevp->SetRenderState( D3DRS_ALPHATESTENABLE, _alphatst );
#elif defined(__RendererGL_1_1_H__)
	if ( _alphatst )glEnable( GL_ALPHA_TEST );
	else			glDisable( GL_ALPHA_TEST );
#endif
}

BlendingAlphaTest* BlendingAlphaTest::GetInstance()
{
    static BlendingAlphaTest* instance=0;
    if(instance==0)
    {
        instance=new BlendingAlphaTest();
        instance->Reset();
     }
    return instance;
}
