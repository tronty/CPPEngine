#ifndef __AttackTimer__
#define __AttackTimer__
/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.other;

#include "../passive/GameController.h";
//import pt.ua.concurrent.CThread;
//import pt.ua.concurrent.Metronome;

//import  java.lang.System.out;

/**
 * Simple timer that uses an Metronome implementation to count duration of an attack time.
 */
 struct AttackTimer /* :  CThread */  {

     // ??? const  Metronome metronome = Metronome(1);
     GameController gc;
     int pause;

    /**
     * Initializes a new attack timer
     *
     * @param gc game controller reference (for calling the end of the attack mode)
     */
     AttackTimer(GameController gc, int attackModeDuration) {
        //assert gc != null;

        this->gc = gc;
        this->pause = attackModeDuration;
    }

    
     void run() {
        LOG_PRINT("AttackingTimer started\n");
#if 0
        for (int i = 0; i < pause; i++) {
            metronome.sync();
        }
#endif
        // once ended, disable attack mode
        gc.disableAttackMode();
    }

    /**
     * Adds more time to the timer
     */
     void addMore(int pause) {

        LOG_PRINT("Added more %d seconds to timer\n", pause);
        this->pause += pause;
    }
};
#endif

