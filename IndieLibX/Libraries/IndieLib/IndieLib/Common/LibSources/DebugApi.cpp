/*****************************************************************************************
 * File: Debug.cpp
 * Desc: Class that manages the output of debuggin information about the execution of IndieLib
 * TODO: quit type numbers and use defines
 *****************************************************************************************/

/*
IndieLib 2d library Copyright (C) 2005 Javier López López (javierlopezpro@gmail.com)

This library is free software you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this library if not, write to the Free Software Foundation, Inc., 59 Temple Place,
Suite 330, Boston, MA 02111-1307 USA 
*/
/*
  Copyright (c) 2015 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
	
*/

#include <CIndieLib.h>
#include <DebugApi.h>

// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
*************************
Init
*************************
*/
bool DebugApi::Init ()
{
	#ifdef _PRINT_

	End ();
	InitVars ();
#if 0
	// File
	std::string f1="/IndieLibX.debug.log";
	std::string f2=stx_convertpath(f1);
	const char* f3=f2.c_str();
	
	mCout = new ofstream (f3, ios::out);
#endif
	// Time
	time_t mT;
	time (&mT);
	struct tm *mPetm = localtime (&mT);

	// :D

	LOG_PRINT("\n");
	LOG_PRINT("                         ''~``                         \n");
	LOG_PRINT("                        ( o o )                        \n");
	LOG_PRINT("+------------------.oooO--(_)--Oooo.------------------+\n");
	LOG_PRINT("|                                                     |\n");
	LOG_PRINT("|                ®-------------------®                |\n");
	LOG_PRINT("|                | I N D I E  L I B  |                |\n");
	LOG_PRINT("|                ®-------------------®                |\n");
	LOG_PRINT("|                    .oooO                            |\n");
	LOG_PRINT("|                    (   )   Oooo.                    |\n");
	LOG_PRINT("+---------------------\\ (----(   )--------------------+\n");
	LOG_PRINT("                       \\_)    ) /                      \n");
	LOG_PRINT("                             (_/                       \n");
	LOG_PRINT("\n");
	LOG_PRINT("\n");
	

	LOG_PRINT("[Debug.log]: (\n");
	// Date
    const char *mDs [7] = {"Sunday","Monday","Tuesday","Wednesday","Thrusday","Friday","Saturday"};
    const char *mMs [12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

	// Date
	LOG_PRINT("%s%d%s%d", mDs [mPetm->tm_wday], mPetm->tm_mday, mMs [mPetm->tm_mon], mPetm->tm_year+1900);
	LOG_PRINT("\n");
	LOG_PRINT("\n");

#endif

	mOk = true;

	return mOk;
}


/*
*************************
End
*************************
*/
void DebugApi::End()
{
	if (mOk)
	{
		//mCout->close ();
		FreeVars ();
		mOk = false;
	}
}


// --------------------------------------------------------------------------------
//							         Public methods
// --------------------------------------------------------------------------------


/*
*************************
Header and message
*************************
*/
void DebugApi::Header (const char *pDataChar, int pType)
{
	#ifdef _PRINT_
	if (!mOk) return;

	switch (pType)
	{
		// Ok
		case (1):
		{
			// Line
			LOG_PRINT("          ");
			LOG_PRINT(" [  OK   ] ");
			Advance ();
			LOG_PRINT("%c\n", pDataChar);

			break;
		}
		// Error
		case (2):
		{
			// Line
			LOG_PRINT("          ");
			LOG_PRINT(" [ ERROR ] ");
			Advance ();
			LOG_PRINT("%c\n", pDataChar);

			// If we are inside a BEGIN / END, we go out
			if (mDepth > 0)
			{
				// Going back
				mDepth -= ESP;
				// Close bracket
				//LOG_PRINT("");                     ";
				Advance ();
				LOG_PRINT("}\n");

				// Line
				WriteTime ();
				LOG_PRINT(" [  END  ] ");
				Advance ();
				LOG_PRINT("Error occurred");

				// Measure the time between BEGIN and END
				unsigned int mElapsedTime = timeGetTime() - mTableTime [(mDepth + ESP) / ESP];
				if (mElapsedTime < 0) mElapsedTime = 0; // Medida de seguridad
				LOG_PRINT(" [Elaped time = %d seg]\n", mElapsedTime * 0.001f);

				// Line jump after BEGIN/END
				if (!mDepth)
				{
					LOG_PRINT("---------------------------------------------------------------------\n");
				}
			}

			break;

		}
		// Info
		// Info dosen't make a line jump in order DataChar and DataInt could write just after that line
		case (3):
		{
			// Line
			LOG_PRINT("          ");
			LOG_PRINT(" [ INFO  ] ");
			Advance ();
			LOG_PRINT("%c", pDataChar);

			break;
		}
		// Warning
		case (4):
		{
			// Line
			LOG_PRINT("          ");
			LOG_PRINT(" [WARNING] ");
			Advance ();
			LOG_PRINT("%c\n", pDataChar);

			break;
		}
		// Begin
		case (5):
		{
			// Line
			WriteTime ();
			LOG_PRINT(" [ BEGIN ] ");
			Advance ();
			LOG_PRINT("-- %c --\n", pDataChar);

			// Open brackets
			LOG_PRINT("                     ");
			Advance ();
			LOG_PRINT("{\n");

			// Advance
			mDepth += ESP;

			// Store the current time in the time table
			mTableTime [mDepth / ESP] = timeGetTime();

			break;
		}
		// End
		case (6):
		{
			// Going back
			mDepth -= ESP;
			// Close bracket
			LOG_PRINT("                     ");
			Advance ();
			LOG_PRINT("}\n");

			// Line
			WriteTime ();
			LOG_PRINT(" [  END  ] ");
			Advance ();
			
			LOG_PRINT("%c", pDataChar);

			// Measure the time between BEGIN and END
			unsigned int mElapsedTime = timeGetTime() - mTableTime [(mDepth + ESP) / ESP];
			if (mElapsedTime < 0) mElapsedTime = 0; // Security Measure
			LOG_PRINT(" [Elapsed time = %d seg]\n", mElapsedTime * 0.001f);

			// Line jump after BEGIN/END
			if (!mDepth)
			{
				LOG_PRINT("---------------------------------------------------------------------\n");
			}

			break;
		}
	}

#endif
}


/*
*************************
Writes a string
*************************
*/
void DebugApi::DataChar (const char *pDataChar, bool pFlag)
{
	#ifdef _PRINT_
	if (!mOk) return;

	LOG_PRINT(" %c", pDataChar);
	// Line jump
	if (pFlag)
		LOG_PRINT("\n");
	

#endif	
}


/*
*************************
Writes an integer
*************************
*/
void DebugApi::DataInt (int pDataInt, bool pFlag)
{
	#ifdef _PRINT_
	if (!mOk) return;

	LOG_PRINT(" %d", pDataInt);
	// Jump line
	if (pFlag)
		LOG_PRINT("\n");
	

#endif	
}


/*
*************************
Writes a float
*************************
*/
void DebugApi::DataFloat (float pDataFloat, bool pFlag)
{
	#ifdef _PRINT_
	if (!mOk) return;

	LOG_PRINT(" %f", pDataFloat);
	// Line jump
	if (pFlag)		
		LOG_PRINT("\n");
	

#endif	
}


/*
*************************
Writes a __DWORD__
*************************
*/
void DebugApi::DataDWORD (__DWORD__ pDataDWORD, bool pFlag)
{
	#ifdef _PRINT_
	if (!mOk) return;

	LOG_PRINT(" %d", pDataDWORD);
	// Jump line
	if (pFlag)		
		LOG_PRINT("\n");


#endif		
}


/*
*************************
Writes a signal (for debugging purposes)
*************************
*/
void DebugApi::BreakPoint ()
{
	#ifdef _PRINT_
	if (!mOk) return;

	LOG_PRINT("Abracadabra\n");
	

#endif
}


/*
*************************
Duplicates an string
*************************
*/
char *DebugApi::DuplicateCharString (const char* charString)
{
    if (!charString)
		return 0;

	return stx_strdup(charString);
}

// --------------------------------------------------------------------------------
//										 Private methods
// --------------------------------------------------------------------------------

/*
*************************
Writes the current time
*************************
*/
void DebugApi::WriteTime ()
{
	#ifdef _PRINT_
	time_t t;
	time(&t);
	struct tm* petm = localtime(&t);

	// Hour
	LOG_PRINT("[");

	// Hours
	if (petm->tm_hour < 10)
		LOG_PRINT("0\n");
	LOG_PRINT("%d:", petm->tm_hour);

	// Minutes
	if (petm->tm_min < 10)
		LOG_PRINT("0\n");
	LOG_PRINT("%d:", petm->tm_min);

	// Secods
	if (petm->tm_sec < 10)
		LOG_PRINT("0\n");
	LOG_PRINT("%d:", petm->tm_sec);

#endif
}


/*
*************************
Advance as many spaces as Depth
*************************
*/
void DebugApi::Advance ()
{
	#ifdef _PRINT_
	for (int i = 0; i < mDepth; i++)
		LOG_PRINT(" ");

#endif
}


/*
*************************
Starts to measure the time
*************************
*/
void DebugApi::Start ()
{
	mTime = timeGetTime();
}


/*
*************************
Stops measuring the time
*************************
*/
void DebugApi::Stop ()
{
	#ifdef _PRINT_
	unsigned int mElapsedTime = mTime - timeGetTime();
	if (mElapsedTime < 0) mElapsedTime = 0;
	mElapsedTime = mElapsedTime * (unsigned int) 0.001f;
	LOG_PRINT(" %d\n",  mElapsedTime);

#endif
}


/*
*************************
Draws all the characteres, included UNICODE
*************************
*/
void DebugApi::AllFont ()
{
	#ifdef _PRINT_
	for (int i = 0; i < 256; i++)
		LOG_PRINT("%c",  (char) i);
	LOG_PRINT("\n");

#endif
	
}


/*
*************************
Init vars
*************************
*/
void DebugApi::InitVars ()
{
	mDepth = 0;
	mTime = 0;

	for (int i = 0; i < 16; i++)
		mTableTime [i] = 0;
}


/*
*************************
Free memory
*************************
*/
void DebugApi::FreeVars ()
{
	//Dispose (mCout);
}


