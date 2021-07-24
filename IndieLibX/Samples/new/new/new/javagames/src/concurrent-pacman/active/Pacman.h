#ifndef __Pacman__
#define __Pacman__
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.active;

#include "passive/GameController.h";
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
        super(name, symbol, gc, pos, speed);
        //maze.board.draw(CircleGelem(Color.BLACK, 100), pos.y, pos.x, 1);

    }

    
     void run() {
        //System.LOG_PRINT(super.getName() + " started");
            gc.reportPosition(initPos);

            while (!searchPath(1, initPos)) {
                //LOG_PRINT("no solutions"); // note that pacman levels up before
                searchPath(1, initPos);
                pathLog = std::map<>();
            }
    }

    bool searchPath(int distance, D3DXFROMWINEVECTOR2 pos) {

        if (gc.validPosition(pos) && gc.isRoad(pos))
            super.searchPath(distance, pos);

        return false;
    }

    /**
     * No special modes for pacman when attack mode.
     */
    
     void attackMode() {
        throw UnsupportedOperationException("Pacman has no attack mode implemented.");
    }


}
#endif

