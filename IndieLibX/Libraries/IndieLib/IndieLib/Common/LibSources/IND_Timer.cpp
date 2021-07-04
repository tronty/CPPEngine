#include <CIndieLib.h>
/*****************************************************************************************
 * File: IND_Timer.h
 * Desc: Class for measuring the pass of time
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

// ----- Includes -----


#include <CIndieLib.h>


#include <IND_Timer.h>


// --------------------------------------------------------------------------------
//							  Initialization / Destruction
// --------------------------------------------------------------------------------

/*
*************************
Init
*************************
*/
IND_Timer::IND_Timer ()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

// --------------------------------------------------------------------------------
//									   Public methods
// --------------------------------------------------------------------------------

/*!
\b Operation:

This function initializes the timer. Just after calling this method the timer will start to measure
the time.
*/
void IND_Timer::Start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
	startTicks = timeGetTime();
}


/*!
\b Operation:

This method stops the timer.
*/
void IND_Timer::Stop()
{
    // Stop the timer
    started = false;

    // Unpause the timer
    paused = false;
}


/*!
\b Operation:

This method pauses the timer.
*/
void IND_Timer::Pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
		pausedTicks = timeGetTime() - startTicks;
    }
}


/*!
\b Operation:

This method unpauses the timer.
*/
void IND_Timer::Unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
		startTicks = timeGetTime() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

/*!
\b Operation:

Returns the number of millisecods passed since the last call to IND_Timer::Start() or
IND_Timer::Init()
*/
unsigned int IND_Timer::GetTicks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
			return timeGetTime() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

