#ifndef __Entity__
#define __Entity__
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.active;

#include "passive/GameController.h";
//import pt.ua.concurrent.CThread;

//import java.awt.*;
//import java.util.Collections;
//import java.util.std::stack;
//import java.util.concurrent.std::map;

/**
 * Entity is an  struct that can represent an active entity in the map
 * Shared algorithms already implemented
 */
  struct Entity /* :  CThread  */ {

     const D3DXFROMWINEVECTOR2 initPos;
     const int initialSpeed;
     const char initialSymbol;
    const GameController gc;
    const char markedStartSymbol;
     const char startSymbol;
     const char markedPositionSymbol = '+';
     const char actualPositionSymbol = 'o';
     volatile D3DXFROMWINEVECTOR2 lastPos;
     char symbol;
     bool underAttack = false;
    std::map<D3DXFROMWINEVECTOR2, Character> pathLog;
    int speed;


    /**
     * Initializes a new Entity
     *
     * @param name   of the entity
     * @param symbol respective symbol that represents the entity
     * @param gc     game controller
     * @param pos    initial position
     * @param speed  of the entity
     */
    Entity(std::string name, char symbol, GameController gc, D3DXFROMWINEVECTOR2 pos, int speed) {
        super(name);
        //assert gc != null;
        //assert pos != null;
        //assert speed > 0;

        this->symbol = symbol;
        initialSymbol = symbol;
        this->gc = gc;
        lastPos = pos;
        this->initPos = pos;
        this->speed = speed;
        initialSpeed = speed;

        this->startSymbol = symbol;
        this->markedStartSymbol = symbol;

        pathLog = std::map<>();
    }

    
      void run();

    /**
     * Randomly walks around the map
     *
     * @param distance to jump from the current location
     * @param pos      position to travel to
     * @return true if a objective was found
     */
    bool searchPath(int distance, D3DXFROMWINEVECTOR2 pos) {
        //assert distance > 0;
        //assert pos != null;
        //assert super.isAlive();

        bool result = false;

        if (freePosition(pos) && !isInterrupted()) {
            CThread.pause(speed);

            markPosition(pos);

            D3DXFROMWINEVECTOR2 lp = (D3DXFROMWINEVECTOR2) pos.clone();
            pos = gc.reportPosition(pos);
            //assert gc.isRoad(pos);

            if (!lp.equals(pos)) {
                markPosition(lastPos);
                markPosition(pos);
            }

            std::stack<D3DXFROMWINEVECTOR2> shuffleDirections = shuffleDirections();

            while (!shuffleDirections.isEmpty()) {
                if (searchPath(distance + 1, getDirection(pos, shuffleDirections.pop()))) {
                    result = true;
                }
            }
            CThread.pause(speed);

            unmarkPosition(pos);
            pos = gc.reportPosition(pos);
            //assert gc.isRoad(pos);
            unmarkPosition(pos);
        }

        return result;
    }

    /**
     * Shuffles the direction of the movement. Making it more random
     *
     * @return directions shuffled
     */
     std::stack<D3DXFROMWINEVECTOR2> shuffleDirections() {
        std::stack<D3DXFROMWINEVECTOR2> directions = std::stack<>();
        directions.push( D3DXFROMWINEVECTOR2(-1, 0));
        directions.push( D3DXFROMWINEVECTOR2(0, +1));
        directions.push( D3DXFROMWINEVECTOR2(0, -1));
        directions.push( D3DXFROMWINEVECTOR2(+1, 0));
        Collections.shuffle(directions);
        return directions;
    }

    /**
     * Computes the next D3DXFROMWINEVECTOR2 position according to the movement direction
     *
     * @param a actual position
     * @param b direction of the movement
     * @return position of the desired movement
     */
     D3DXFROMWINEVECTOR2 getDirection(D3DXFROMWINEVECTOR2 a, D3DXFROMWINEVECTOR2 b) {
        return  D3DXFROMWINEVECTOR2(a.x + b.x, a.y + b.y);
    }

    /**
     * Checks if the position is a valid, free position to move
     *
     * @param pos to check
     * @return if valid
     */
    bool freePosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        char rs = roadSymbol(pos);

        return rs == ' ' // road
                || rs == '$' // bonus
                || rs == '.' // point
                || rs == '?' // portal
                || rs == '!' // portal destination
                || gc.symbolIsEntity(rs);
    }

    /**
     * Marks the travelled path with an symbol
     *
     * @param pos position to mark
     */
     void markPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        if (isStartPosition(pos)) {
            putRoadSymbol(pos, markedStartSymbol);
        } else {
            putRoadSymbol(pos, actualPositionSymbol);
        }

    }

    /**
     * Unmarks an travelled path
     *
     * @param pos position to unmark
     */
     void unmarkPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        if (!isStartPosition(pos)) {
            putRoadSymbol(pos, markedPositionSymbol);
        }
    }

    /**
     * Checks if the position is the start position
     *
     * @param pos position to check
     * @return true if is the start
     */
     bool isStartPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        return roadSymbol(pos) == startSymbol
                || roadSymbol(pos) == markedStartSymbol;
    }

    /**
     * Checks the symbol on the road
     * This method caches the map from the map
     *
     * @param pos position to check
     * @return symbol of the road
     */
    char roadSymbol(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        return pathLog.computeIfAbsent(pos, (t) -> gc.CachedRoadSymbol(pos));
    }

    /**
     * Puts an symbol in the road in the local cached map
     *
     * @param pos    position to mark
     * @param symbol symbol to use in the mark
     */
     void putRoadSymbol(D3DXFROMWINEVECTOR2 pos, char symbol) {
        //assert pos != null;
        //assert gc.isRoad(pos);

        pathLog.put(pos, symbol);
    }

    /**
     * When pacman is attacking what behaviour to have
     */
      void attackMode();

    /**
     * Returns the entity speed
     *
     * @return speed of the entity
     */
     int getInitialSpeed() {
        return initialSpeed;
    }

    /**
     * Checks if entity is a Ghost
     *
     * @return true if ghost.
     */
     bool isGhost() {
        return this instanceof Ghost;
    }

    /**
     * Checks if entity is under attack
     *
     * @return true if under attack
     */
     bool underAttack() {
        return underAttack;
    }

}
#endif

