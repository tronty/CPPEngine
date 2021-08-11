/*****************************************************************************************
 * File: Global.h
 * Desc: Global class
 *****************************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// ----- Includes -----

#include "DebugApi.h"
//#include "basetsd.h"

// ----- Global class -----

DebugApi * GetDebugAPI();
int Getbbb();
int Getmmm();
D3DXFROMWINEMATRIX INDIELIBAPI GetMVP();
class INDIELIBAPI CIndieLib;

#endif // _GLOBAL_H_
