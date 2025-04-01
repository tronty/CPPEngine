#ifndef __Pacman__
#define __Pacman__
/*
  Copyright (c) 2021 Tuomas Roenty   http://www.tuomasronty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.active;

#include "../active/Entity.h";
#include "../passive/GameController.h";
//import pt.ua.concurrent.ThreadInterruptedException;

//import java.awt.*;
//import java.util.concurrent.std::map;

//import  java.lang.System.out;

/**
 * Implements a Pacman. Simplest active entity
 */
 struct Pacman :  public Entity {

     const char endSymbol = '$';

     Pacman(std::string name, GameController gc, char symbol, D3DXFROMWINEVECTOR2 pos, int speed) {
        Entity(name, symbol, gc, pos, speed);
        //maze.board.draw(CircleGelem(Color.BLACK, 100), pos.y, pos.x, 1);

    }

    
     void run() {
        //LOG_PRINT("%s started\n", Entity::getName());
            gc.reportPosition(initPos);

            while (!searchPath(1, initPos)) {
                //LOG_PRINT("no solutions\n"); // note that pacman levels up before
                searchPath(1, initPos);
                pathLog = std::map<D3DXFROMWINEVECTOR2, char>();
            }
    }

    bool searchPath(int distance, D3DXFROMWINEVECTOR2 pos) {

	if (gc.validPosition(pos) && gc.isRoad(pos))
            Entity::searchPath(distance, pos);

        return false;
    }

    /**
     * No special modes for pacman when attack mode.
     */
    
     void attackMode() {
        //throw UnsupportedOperationException
        LOG_PRINT("Pacman has no attack mode implemented.\n");
    }


};
#endif

