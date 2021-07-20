#ifndef MATHUTILS

#define MATHUTILS

#include <float.h>

#include <math.h>

#include <ctime>

#include <iostream>

//include "Logger.h"
/*

#include "Tuple2.h"

#include "Tuple3.h"

#include "Tuple4.h"

#include "Matrix4.h"

#include "Quaternion.h"

*/

#include <STX/d3dx9mathFromWine.h>
using namespace std;

//include "../../../../vector.h"

template <class T>

inline T clampNS(T x, T amin, T amax)

{

  return (x < amin) ? amin : (x > amax) ? amax : x;

}



/*

    inline D3DXFROMWINEVECTOR3 &clampNS(D3DXFROMWINEVECTOR3 i,float min, float max)

    {

		D3DXFROMWINEVECTOR3 o;

      o.x = i.x > max ? max : i.x < min ? min  : i.x;

      o.y = i.y > max ? max : i.y < min ? min  : i.y;

      o.z = i.z > max ? max : i.z < min ? min  : i.z;

      return o;

    }

*/

struct Tuple2i

{

	Tuple2i()

	{

	x=0;

	y=0;

	}

	Tuple2i(int ax,int ay)

	{

	x=ax;

	y=ay;

	}

	void set(int ax,int ay)

	{

	x=ax;

	y=ay;

	}


    Tuple2i(const Tuple2i &a)

    {

	x=a.x;

	y=a.y;

    }

	Tuple2i operator =(const Tuple2i &a)

    {

	x=a.x;

	y=a.y;

	return *this;

    }

    bool operator ==(const Tuple2i &a)

    {

		return (

	(x==a.x)&&

	(y==a.y));

    }

 int x;

 int y;

};

struct Tuple3i

{

	Tuple3i()

	{

	x=0;

	y=0;

	z=0;

	}

	Tuple3i(int ax,int ay,int az)

	{

	x=ax;

	y=ay;

	z=az;

	}

	void set(int ax,int ay,int az)

	{

	x=ax;

	y=ay;

	z=az;

	}
    Tuple3i(const Tuple3i &a)

    {

	x=a.x;

	y=a.y;

	z=a.z;

    }

    Tuple3i operator =(const Tuple3i &a)

    {

	x=a.x;

	y=a.y;

	z=a.z;

	return *this;

    }

    bool operator ==(const Tuple3i &a)

    {

		return (

	(x==a.x)&&

	(y==a.y)&&

	(z==a.z));

    }

 int x;

 int y;

 int z;

};

struct Tuple4i

{

	Tuple4i()

	{

	x=0;

	y=0;

	z=0;

	w=0;

	}

	Tuple4i(int ax,int ay,int az,int aw)

	{

	x=ax;

	y=ay;

	z=az;

	w=aw;

	}

	void set(int ax,int ay,int az,int aw)

	{

	x=ax;

	y=ay;

	z=az;

	w=aw;

	}

    Tuple4i(const Tuple4i &a)

    {

	x=a.x;

	y=a.y;

	z=a.z;

	w=a.w;

    }

    Tuple4i operator =(const Tuple4i &a)

    {

	x=a.x;

	y=a.y;

	z=a.z;

	w=a.w;

	return *this;

    }

    bool operator ==(const Tuple4i &a)

    {

		return (

	(x==a.x)&&

	(y==a.y)&&

	(z==a.z)&&

	(w==a.w));

    }



 int x;

 int y;

 int z;

 int w;

};



/*

 *Author: Abdul Bezrati

 *Email : abezrati@hotmail.com

 */



#define TWO_PI          6.2831853f

#define EPSILON         0.0001f

#define EPSILON_SQUARED EPSILON*EPSILON

#define RAD2DEG         57.2957f

#define DEG2RAD         0.0174532f

  /*

template <class T>

inline T clampNS(T x, T min, T max)

{

  return (x < min) ? min : (x > max) ? max : x;

}

*//*

inline int round(float f)

{

  return int(f + 0.5f);

}*/



inline float getNextRandom(){

  return float(stx_rand())/RAND_MAX;//??? + 1);

}



inline int getClosest(int arg, int firstVal, int secondVal)

{

  int difference1 = 0,

      difference2 = 0;



  difference1 = abs(arg - firstVal);

  difference2 = abs(arg - secondVal);

  if(difference1 < difference2)

    return firstVal;



  return secondVal;

}


#if 0//ndef __FASTMATH__
#define __FASTMATH__
inline float fastSquareRoot(float x)

{

  __asm{

    fld x;

    fsqrt;

    fstp x;

  }

  return x;

}



inline float fastCos(float x)

{

  __asm{

    fld x;

    fcos;

    fstp x;

  }

  return x;

}



inline float fastSin(float x)

{

  __asm{

    fld x;

    fsin;

    fstp x;

  }

  return x;

}



#endif
/*inline float fastSquareRootSSE(float f)       {

  __asm    {

   MOVSS xmm2,f

   SQRTSS xmm1, xmm2

   MOVSS f,xmm1

  }

  return f;

}*/



inline void operator *=(D3DXFROMWINEVECTOR4 &t,const D3DXFROMWINEMATRIX &mat){

      t=D3DXFROMWINEVECTOR4(mat.m[0][0]*t.x + mat.m[1][0]*t.y + mat.m[2][0]*t.z + mat.m[3][0]*t.w,

            mat.m[0][1]*t.x + mat.m[1][1]*t.y + mat.m[2][1]*t.z + mat.m[3][1]*t.w,

            mat.m[0][2]*t.x + mat.m[1][2]*t.y + mat.m[2][2]*t.z + mat.m[3][2]*t.w,

            mat.m[0][3]*t.x + mat.m[1][3]*t.y + mat.m[2][3]*t.z + mat.m[3][3]*t.w);

    }



    inline const float getDistance(const D3DXFROMWINEVECTOR3 &v1, const D3DXFROMWINEVECTOR3 &v2)

    {

      return sqrtf((v2.x - v1.x) * (v2.x - v1.x) +

                   (v2.y - v1.y) * (v2.y - v1.y) +

                   (v2.z - v1.z) * (v2.z - v1.z));

    }

#endif



