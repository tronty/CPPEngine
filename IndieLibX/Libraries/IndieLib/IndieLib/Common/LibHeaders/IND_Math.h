/*****************************************************************************************
/* File: IND_Math.h
/* Dess: ¿Math methods?
/*****************************************************************************************/
 
/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com) 

This library is free software; you can redistribute it and/or modify it under the 
terms of the GNU Lesser General Public License as published by the Free Software 
Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY 
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with 
this library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, 
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	 
*/

#ifndef _IND_MATH_
#define _IND_MATH_


// --------------------------------------------------------------------------------
//									   IND_Math
// --------------------------------------------------------------------------------

class DLL_EXP IND_Math
{
public:

	// ----- Init/End -----

	IND_Math		(): mOk(false)	{ }
	~IND_Math		()				{ End(); }

	bool	Init    ();
	void	End     ();
	bool	IsOk	() { return mOk; }
	
	// ----- Public methods -----
	void 		Reset ();
#ifndef SWIG
	void		RandSeed					(int pNum);
#endif
	INLINE int Rand						(int pIni, int pEnd);
	bool		IsPowerOfTwo				(int pN);
	
private:

	// ----- Private -----
	__DWORD__ m_seed;
	
	bool mOk;

	
	// ----- Private methods -----
	
	void				InitVars		();
	void				FreeVars		();
};
	
#endif // _IND_MATH_
