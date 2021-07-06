/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/

// find .. -type f -name "*.h" -exec grep -l "ifndef SWIG" {} \;

    /* IndieLib.i */
     %module IndieLib
     %include "carrays.i"
     %array_functions(int, intArray);
     %array_functions(float, floatArray);

     %{
     	#include "../../../../Libraries/STX/include/STX/d3dx9mathFromWine.h"
	#include "../../../../Libraries/STX/include/STX/d3dx9mathFromWine.inl"
	//include "../../../../Libraries/STX/include/STX/STXinterfaces.h"

	//D3DXFROMWINEMATRIX stx_GetPivotMatrix(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aT);
     %}

     %include "../../../../Libraries/STX/include/STX/d3dx9mathFromWine.h"
     %include "../../../../Libraries/STX/include/STX/d3dx9mathFromWine.inl"
     //include "../../../../Libraries/STX/include/STX/STXinterfaces.h"

	//D3DXFROMWINEMATRIX stx_GetPivotMatrix(const D3DXFROMWINEVECTOR3& aR, const D3DXFROMWINEVECTOR3& aT);

     %array_functions(D3DXFROMWINEVECTOR2, D3DXFROMWINEVECTOR2Array);

