#ifndef __Ghost__
#define __Ghost__
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.active;

#include "passive/GameController.h";
//import pt.ua.concurrent.CThread;
//import pt.ua.concurrent.ThreadInterruptedException;

//import java.awt.*;
//import java.util.concurrent.std::map;
//import java.util.concurrent.ScheduledExecutorService;
//import java.util.concurrent.TimeUnit;

//import  java.lang.System.err;
//import  java.lang.System.out;

/**
 * Ghost struct. Special cases include trespassing ghost cages and continually running.
 */
 struct Ghost :  public Entity {

     const int attackModeSlowdownFactor;
     const int blinkSpeed;
     const bool alive = true;
     ScheduledExecutorService es;

     Ghost(std::string name, GameController gc, char symbol, D3DXFROMWINEVECTOR2 pos, int speed, int slowdownFactor, int blinkSpeed) {
        super(name, symbol, gc, pos, speed);
        attackModeSlowdownFactor = slowdownFactor;
        this->blinkSpeed = blinkSpeed;
    }


    /**
     * Ghost struct runs around until killed
     */
    
     void run() {
        //LOG_PRINT(super.getName() + " started");
        
            //noinspection InfiniteLoopStatement
            while (alive) {
                searchPath(1, initPos);
                pathLog = std::map<>();
            }
    }

    
    bool searchPath(int distance, D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert distance > 0;

        if (gc.validPosition(pos) && gc.isRoad(pos))
            super.searchPath(distance, pos);

        return false;
    }

    /**
     * Allow to pass ghost cage gate
     *
     * @param pos to check
     * @return true if valid path to wander
     */
    
    bool freePosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        return super.freePosition(pos)
                || roadSymbol(pos) == '%'; // allow passing though ghost cage gate
    }

    /**
     * Schedule threads at a fixed rate that toggle black and white ghost representation
     */
    
     void attackMode() {

        if (es == null || es.isTerminated()) {

            symbol = 'b'; // ghost black
            underAttack = true;

            es = java.util.concurrent.Executors.newScheduledThreadPool(1);

            es.scheduleAtFixedRate(() -> {
                //LOG_PRINT("Toggling " + super.getName() + " symbol");
                if (symbol == 'w')
                    symbol = 'b';
                else
                    symbol = 'w';

            }, 0, blinkSpeed, TimeUnit.MILLISECONDS);

            speed = speed * attackModeSlowdownFactor;

            // wait for attack mode to end.
            CThread(() -> {
                gc.hasAttackModeEnded();
                disableAttackMode();
            }).start();
        }

    }

     void disableAttackMode() {
        //assert !es.isTerminated();

            LOG_PRINT("Ending " + super.getName() + " attack mode");
            speed = speed / attackModeSlowdownFactor;
            underAttack = false;
            es.shutdown();
            es.awaitTermination(1, TimeUnit.SECONDS);

        symbol = markedStartSymbol;
    }

}
#endif

