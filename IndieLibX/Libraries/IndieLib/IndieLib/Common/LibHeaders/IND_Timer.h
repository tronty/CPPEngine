/*****************************************************************************************
/* File: IND_Timer.h
/* Desc: Class for measuring the pass of time
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

#ifndef _IND_TIMER_
#define _IND_TIMER_


// --------------------------------------------------------------------------------
//									   IND_Timer
// --------------------------------------------------------------------------------

/*!
\defgroup IND_Timer IND_Timer
\ingroup Timer
IND_Timer class for measuring the time. Click in IND_Timer to see all the methods of this class.
*/
/*@{*/

/*!
\b IND_Timer is an object that will help you to measure the passing of time. Click in IND_Timer to see all the methods of this class.
*/
class DLL_EXP IND_Timer
{
public:

	// ----- Init/End -----

	IND_Timer		();

	// ----- Public methods -----

    //The various clock actions
    void Start		();
    void Stop		();
    void Pause		();
    void Unpause	();
    
    //Gets the timer's time
    unsigned int GetTicks();
    
    // ----- Public gets -----

	//! This function returns true if the timer has been started
	bool IsStarted	()					{ return started;	}
	//! This function returns true if the timer is paused
	bool IsPaused	()					{ return paused;	}

private:

	// ----- Private -----
	
    //The clock time when the timer started
    int startTicks;
    
    //The ticks stored when the timer was paused
    int pausedTicks;
    
    //The timer status
    bool paused;
    bool started;
};
/*@}*/

#endif // _IND_TIMER_
