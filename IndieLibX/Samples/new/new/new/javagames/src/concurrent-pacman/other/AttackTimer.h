/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.other;

//import pacman.passive.GameController;
//import pt.ua.concurrent.CThread;
//import pt.ua.concurrent.Metronome;

//import  java.lang.System.out;

/**
 * Simple timer that uses an Metronome implementation to count duration of an attack time.
 */
 struct AttackTimer :  CThread {

     const  Metronome metronome = new Metronome(1);
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
        out.println("AttackingTimer started");

        for (int i = 0; i < pause; i++) {
            metronome.sync();
        }

        // once ended, disable attack mode
        gc.disableAttackMode();
    }

    /**
     * Adds more time to the timer
     */
     void addMore(int pause) {

        out.println("Added more " + pause + " seconds to timer");
        this->pause += pause;
    }
}
#endif
