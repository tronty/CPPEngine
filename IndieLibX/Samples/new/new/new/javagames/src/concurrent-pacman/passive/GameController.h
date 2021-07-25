#ifndef __GameController__
#define __GameController__
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
//package pacman.passive;

#include "../active/Entity.h";
#include "../active/Ghost.h";
#include "../active/Pacman.h";
#include "../other/AttackTimer.h";
//import pt.ua.concurrent.CObject;
//import pt.ua.concurrent.CThread;
//import pt.ua.gboard.*;
//import pt.ua.gboard.games.Labyrinth;
//import pt.ua.gboard.games.LabyrinthGelem;

//import java.awt.*;
//import java.io.File;
//import java.util.std::vector;
//import java.util.concurrent.std::map;

//import  java.lang.System.out;

typedef Character char;

/**
 * Central game controller.
 * This struct is responsible for, and not limited to:
 * - Count Pacman points, pacmanLives, etc
 * - Kill and spawn entities
 * - Enable/Disable attack mode
 * - Synchronization of GBoard
 */
 struct GameController /* :  CObject */ {

     const Labyrinth labyrinth;
     const std::vector<D3DXFROMWINEVECTOR2> portalPoints, portalDst;
    // const CopyOnWriteArrayList<Entity> entities = CopyOnWriteArrayList<>();
     const std::vector<Entity> entities;
     const std::map<D3DXFROMWINEVECTOR2, Entity> entityTracker = std::map<D3DXFROMWINEVECTOR2, Entity>();
     const std::map<D3DXFROMWINEVECTOR2, Character> mapCache = std::map<D3DXFROMWINEVECTOR2, Character>();
     const bool GodMode;
     const int attackModeDuration;
     const bool endless;
    // const ReentrantLock lock = ReentrantLock();
    // const Condition attackEnded = lock.newCondition();
    // const Condition hasGameEnded = lock.newCondition();
     int totalNumberOfPoints;
     AttackTimer timer;
     int points = 0;
     bool attackInProgress = false;
     int pacmanLives;
     bool gameInProgress = true;


    /**
     * Initializes a new game controller.
     */
     GameController(int numberPacmanLives,
                          int attackModeDuration,
                          bool endless) {

        pacmanLives = numberPacmanLives;
        this->GodMode = numberPacmanLives == -1;
        this->attackModeDuration = attackModeDuration;
        this->endless = endless;

        LabyrinthGelem.setShowRoadBoundaries();

        std::vector<char> allSymbols = {
                // main ghosts
                'C', // inky
                'P', // pinky
                'O', // clyde
                'R', // blinky
                'X', // pacman
                // ghosts representations
                'w', // ghost white
                'b', // ghost black
                'G', // generic ghost
                // game elements
                '.', // point
                '$', // bonus
                // map helpers
                '%', // ghost cage
                '+', // markedPositionSymbol
                'o', // actualPositionSymbol
                '?', // portal
                '!' // portal destination
        };
#if 0
        File f = File("/new/javagames/concurrent-pacman/board.txt");
        if (!f.exists()) {
            LOG_PRINT("Missing resources folder\n");
            LOG_PRINT("Looking at:%s\n", f.getAbsolutePath());
            stx_exit(1);
        }
#endif
        labyrinth = new Labyrinth("/new/javagames/concurrent-pacman/board.txt", allSymbols);

        std::vector<Gelem> allGelems = {
                // entities
                ImageGelem("/new/javagames/concurrent-pacman/inky.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/pinky.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/clyde.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/blinky.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/pacman_r.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/attack_w.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/attack_b.png", labyrinth.board, 100),
                ImageGelem("/new/javagames/concurrent-pacman/generic.png", labyrinth.board, 100),
                // game elements
                StringGelem("" + '.', Color.black),
                CircleGelem(Color.blue, 65),
                // mapPath helpers
                CircleGelem(Color.GREEN, 35),
                CircleGelem(Color.GREEN, 45),
                CircleGelem(Color.BLACK, 35),
                CircleGelem(Color.LIGHT_GRAY, 5),
                CircleGelem(Color.LIGHT_GRAY, 5),
                CircleGelem(Color.ORANGE, 35)};

        // attach all gelems to the map
        for (int i = 0; i < allSymbols.length; i++) {
            labyrinth.attachGelemToRoadSymbol(allSymbols[i], allGelems[i]);
        }

        // query the portal destinations and points
        portalPoints = labyrinth.roadSymbolPositions('?');
        portalDst = labyrinth.roadSymbolPositions('!');

        // check total number of points for pacman to collect
        totalNumberOfPoints = getPositions('.').length;
        LOG_PRINT("Game maximum number of points: %d\n", totalNumberOfPoints);

        // draw "P" points on the map
        labyrinth.board.draw(StringGelem("P:", Color.white), 32, 4, 1);
    }

    /**
     * Receives a report from an entity where it wants to move.
     * This movement triggers collisions with other entities, killing, spawning, points calculation etc
     *
     * @param pos to move the entity to
     * @return new position, if teleported by the portal
     */
     /* synchronized */ D3DXFROMWINEVECTOR2 reportPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert isRoad(pos);

        Entity entity;// ??? = (Entity) CThread.currentThread();
        //assert entity.isAlive();

        // portal
        if (pos.equals(portalPoints[0]) || pos.equals(portalPoints[1])) {

            // remove presence from structures
            entityTracker.remove(entity.lastPos);
            labyrinth.putRoadSymbol(entity.lastPos.y, entity.lastPos.x, ' ');

            pos = (pos.equals(portalPoints[1])) ? portalDst[0] : portalDst[1]; // transport entity to portal destination

            entityTracker.put(pos, entity); // re-add presence
            entity.lastPos = (pos.equals(portalDst[1])) ? portalPoints[1] : portalPoints[0]; // update last position to portal
        }

        char mapSymbol = RealRoadSymbol(pos);

        // clean up our presence on map. check if cell is occupied from some entity
        Entity entity1 = entityTracker.get(pos);
        entityTracker.remove(pos);
        entityTracker.remove(entity.lastPos);

        // cell is occupied. did pacman and a ghost crossed each other?
        if (entity1 != null) {
            if (entity.isGhost() && !entity1.isGhost()
                    || !entity.isGhost() && entity1.isGhost()) {
                LOG_PRINT("I am:%s killing %s %d on cache:%s\n", entity.getName(), entity1.getName(), pos, entity1.getName());

                if (!entity.isGhost()) {
                    if (entity1.underAttack() || GodMode) {
                        spawnNewEntity(entity1); // kill entity
                    } else {
                        labyrinth.putRoadSymbol(entity.lastPos.y, entity.lastPos.x, ' ');
                        spawnNewEntity(entity); // pacman dies
                    }

                } else {
                    if (entity.underAttack() || GodMode) {
                        spawnNewEntity(entity); // we, ghost, die
                    } else {
                        labyrinth.putRoadSymbol(entity1.lastPos.y, entity1.lastPos.x, ' ');
                        spawnNewEntity(entity1); // we kill pacman
                    }
                }

                //labyrinth.putRoadSymbol(pos.y, pos.x, mapCache.computeIfAbsent(pos, t -> ' '));
                labyrinth.putRoadSymbol(pos.y, pos.x, mapCache.computeIfAbsent(pos, t -> symbolIsEntity(mapSymbol) ? mapSymbol : ' ')); // After killing, restore map

                if (entity.isInterrupted()) return pos;

            }
        }

        // pacman login
        if (!entity.isGhost()) {
            if (RealRoadSymbol(pos) == '.') { // point counting logic
                points++;

                if ((points % 30) == 0) LOG_PRINT("Points %d/%d\n", points, totalNumberOfPoints);

                drawPoints();

                if (points == totalNumberOfPoints && !endless)
                    endGame();

            } else if (RealRoadSymbol(pos) == '$') { // bonus logic
                attackInProgress = true;
                entities.stream().filter(Entity::isGhost).forEach(Entity::attackMode);
                startAttackTimer();
            }

            mapCache.put(pos, ' '); // collectible was collected. mapCache is just road now
        } else {
            // cache the current bonus/point under target position
            mapCache.computeIfAbsent(pos, t -> symbolIsEntity(mapSymbol) ? ' ' : mapSymbol);
        }

        paintPosition(pos); // representation on map
        entityTracker.put(pos, entity);

        return pos;
    }

    /**
     * Draw points in the GBoard
     */
     /* synchronized */ void drawPoints() {
        //assert points <= totalNumberOfPoints;
        //assert points >= 0;
        //assert labyrinth.numberOfLines >= 32; // TODO remove hardcoded point positions

        std::string sPoints = StringBuilder(std::string.valueOf(points)).reverse().toString();
        labyrinth.board.draw(FilledGelem(Color.blue, 100), 32, 5, 1);
        labyrinth.board.draw(FilledGelem(Color.blue, 100), 32, 6, 1);
        labyrinth.board.draw(FilledGelem(Color.blue, 100), 32, 7, 1);

        for (int i = 0; i < sPoints.length(); i++) {
            labyrinth.board.draw(StringGelem(std::string.valueOf(sPoints.charAt(i)), Color.white), 32, 7 - i, 1);
        }

    }

    /**
     * Spawns a new cloned entity
     *
     * @param entity to clone
     */
     /* synchronized */ void spawnNewEntity(Entity entity) {
        //assert entity != null;
        //assert entity.isAlive();
        //assert !entity.isInterrupted();
        //assert entities.contains(entity);

        entities.remove(entity);
        entity.interrupt();

        if (entity.isGhost()) {
            LOG_PRINT("Spawning new %s\n", entity.getName());
            Ghost ghost = (Ghost) entity;
            ghost = Ghost(entity.getName(), this, entity.initialSymbol, entity.initPos, entity.initialSpeed, ghost.attackModeSlowdownFactor, ghost.blinkSpeed);

            entities.add(ghost);
            ghost.start();
            LOG_PRINT("%s spawned at %d\n", entity.getName(), entity.initPos);
        } else {
            if(!endless) pacmanLives--;
            if (pacmanLives > 0) {
                LOG_PRINT("Pacman has %d pacmanLives left.\n", pacmanLives);
                Pacman pm = Pacman(entity.getName(), this, entity.initialSymbol, entity.initPos, entity.getInitialSpeed());
                entities.add(pm);
                pm.start();
            } else {
                endGame();
            }
        }
    }

    /**
     * End the game, interrupts all entities and signals who's waiting for the end of the game.
     */
     /* synchronized */ void endGame() {
        //assert gameInProgress;

        entities.forEach(Thread::interrupt);
        gameInProgress = false;
        broadcast();
    }

    /**
     * Waiting point for the end of the made
     *
     * @return true if pacman won, false otherwise
     */
     /* synchronized */ bool waitingForGameToEnd() {
        //assert gameInProgress;

        while (gameInProgress)
            await();

        LOG_PRINT("Final points:%d\n", points);
        return points == totalNumberOfPoints;
    }

    /**
     * Paints an entity in the map
     *
     * @param pos to paint
     */
     /* synchronized */ void paintPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert isRoad(pos);

        Entity entity = (Entity) CThread.currentThread();

        labyrinth.putRoadSymbol(entity.lastPos.y, entity.lastPos.x, mapCache.computeIfAbsent(entity.lastPos, t -> RealRoadSymbol(pos)));
        mapCache.computeIfAbsent(pos, t -> RealRoadSymbol(pos));
        labyrinth.putRoadSymbol(pos.y, pos.x, entity.symbol);

        if (!entity.lastPos.equals(pos))
            entity.lastPos = pos;
    }


    /**
     * Starts a timer counting the duration of an attack.
     * An attack is when pacman eats a bonus and ghosts can be killed by pacman.
     */
     /* synchronized */ void startAttackTimer() {
        //assert attackInProgress;

        if (timer == null || !timer.isAlive()) {
            timer = AttackTimer(this, attackModeDuration);
            timer.start();
        } else {
            timer.addMore(attackModeDuration);
        }
    }

    /**
     * Synchronized GBoard's roadSymbol() method
     *
     * @param pos to get the road symbol
     * @return char road symbol
     */
     /* synchronized */ char RealRoadSymbol(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert isRoad(pos);

        return labyrinth.roadSymbol(pos.y, pos.x);
    }

     /* synchronized */ char CachedRoadSymbol(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;
        //assert isRoad(pos);

        return mapCache.computeIfAbsent(pos, t -> RealRoadSymbol(pos));
    }

    /**
     * Synchronized GBoard's isRoad() method
     *
     * @param pos to check if is road
     * @return true if it is a road position, otherwise it returns false
     */
     /* synchronized */ bool isRoad(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;

        return labyrinth.isRoad(pos.y, pos.x);
    }

    /**
     * Synchronized GBoard's validPosition() method
     *
     * @param pos to check if valid
     * @return true if position is valid, otherwise it returns false
     */
     /* synchronized */ bool validPosition(D3DXFROMWINEVECTOR2 pos) {
        //assert pos != null;

        return labyrinth.validPosition(pos.y, pos.x);
    }

    /**
     * Synchronized GBoard roadSymbolPositions() method
     *
     * @param symbol to road
     * @return array of positions
     */
     /* synchronized */ std::vector<D3DXFROMWINEVECTOR2> getPositions(char symbol) {

        return labyrinth.roadSymbolPositions(symbol);
    }

    /**
     * Disables attack mode and broadcasts signal to everyone waiting for its end
     */
     /* synchronized */ void disableAttackMode() {
        //assert attackInProgress;

        attackInProgress = false;
        broadcast();

    }

    /**
     * Waiting point for the end of an attack
     */
     /* synchronized */ void hasAttackModeEnded() {
        //assert attackInProgress;

        while (attackInProgress)
            await();

    }

    /**
     * Attaches extra entities to the game
     *
     * @param entity to attach
     */
     /* synchronized */ void attachExtraEntity(Entity entity) {

        labyrinth.putRoadSymbol(entity.initPos.y, entity.initPos.x, entity.symbol);
        mapCache.put(entity.initPos, ' ');
        entities.add(entity);
        totalNumberOfPoints = getPositions('.').length;
        LOG_PRINT("Updated maximum number of points to %d\n", totalNumberOfPoints);
    }

    /**
     * Checks if the symbol is an active entity
     *
     * @param symbol to check
     * @return true if active entity
     */
     /* synchronized */ bool symbolIsEntity(char symbol) {
        return symbol == 'C' // inky
                || symbol == 'P' // pinky
                || symbol == 'O' // clyde
                || symbol == 'R' // blinky
                || symbol == 'X' // pacman
                || symbol == 'w' // ghost white
                || symbol == 'b' // ghost black
                || symbol == 'G'; // generic ghost
    }

};
#endif

