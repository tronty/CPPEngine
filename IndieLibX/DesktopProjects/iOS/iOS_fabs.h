/*
  Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

#ifndef _stx_stdinc_h
#define _stx_stdinc_h

#ifdef __cplusplus
namespace std
{
	inline double fabs(double f)
	{
    		if (f < 0) return -f;
    		return f;
	}
};

#endif
#endif

