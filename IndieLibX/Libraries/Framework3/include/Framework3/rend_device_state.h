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

#ifndef RENDDEVICESTATE_H
#define RENDDEVICESTATE_H

#include <Framework3/IRenderer.h>
#include <Framework3/RendererHLSLCg.h>

namespace PSYSGFX
{
enum TESTFUNC {
	TESTFUNC_NEVER,
	TESTFUNC_LESS,
	TESTFUNC_EQUAL,
	TESTFUNC_LESSEQUAL,
	TESTFUNC_GREATER,
	TESTFUNC_NOTEQUAL,
	TESTFUNC_GREATEREQUAL,
	TESTFUNC_ALWAYS
};
enum BLENDFCT {
	BLENDFCT_ZERO,
	BLENDFCT_ONE,
	BLENDFCT_SRC_COLOR,
	BLENDFCT_ONE_MINUS_SRC_COLOR,
	BLENDFCT_SRC_ALPHA,
	BLENDFCT_ONE_MINUS_SRC_ALPHA,
	BLENDFCT_DST_ALPHA,
	BLENDFCT_ONE_MINUS_DST_ALPHA,
	BLENDFCT_DST_COLOR,
	BLENDFCT_ONE_MINUS_DST_COLOR,
	BLENDFCT_SRC_ALPHA_SATURATE
};
};

//==================================================================
///
//==================================================================
//class RendDevice;

//==================================================================
///
//==================================================================
struct BlendingAlphaTest//RendDeviceState
{
	bool					_blend;
	unsigned int	_blend_src;
	unsigned int	_blend_dst;

	bool					_alphatst;
	unsigned int		_alphatst_func;
	float					_alphatst_ref;

public:
        static BlendingAlphaTest* GetInstance();
private:
	BlendingAlphaTest()
	{
	}
    virtual ~BlendingAlphaTest()
    {
    }
public:
	void Reset();

	void SetBlending( bool onoff );
	void SetBlending( bool onoff, unsigned int src, unsigned int dst );
	void SetAlphaTest( bool onoff, unsigned int tstfunc, float refparam );
	void SetAlphaTest( bool onoff );
};

#endif
