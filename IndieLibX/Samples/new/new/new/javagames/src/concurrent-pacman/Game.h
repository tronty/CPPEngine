#ifndef __Game__
#define __Game__
/*
  Copyright (c) 2021 Tommi Roenty   http://www.tommironty.fi/
  Licensed under The GNU Lesser General Public License, version 2.1:
      http://opensource.org/licenses/LGPL-2.1
*/
#include <Framework3/IRenderer.h>
    ////package pacman;

#include "active/Entity.h";
#include "active/Ghost.h";
#include "active/Pacman.h";
#include "passive/GameController.h";
////import pt.ua.concurrent.CThread;

////import java.awt.*;
////import java.util.std::vector;
////import java.util.concurrent.ThreadLocalRandom;

////import  java.lang.System.out;

 struct Game {

    /**
     * @param args the command line arguments
     */
      void main(String[] args) {

        if (args.length != 1) {
            printHelp();
            System.exit(1);
        }

        int lives = 3;
        int attackDuration = 5000;
        int slowdownFactor = 3;
        int blinkSpeed = 500;
        int extraGhots = 0;
        int extraPacmans = 0;
        bool endless = false;
        std::vector<Entity> entities = new std::vector<>();

        switch (args[0].charAt(0)) {
            case '1': {
                // normal mode
                // no changes to default
                break;
            }
            case '2': {
                // aggressive mode. 100 lives, 16 ghosts
                lives = 100;
                extraGhots = 12;
                attackDuration = 15000;
                break;
            }
            case '3': {
                // endless
                endless = true;
                break;
            }
            case '4': {
                // crazy mode
                lives = -1;
                extraPacmans = 31;
                extraGhots = 28;
                endless = true;
                break;
            }
            case '5': {
                lives = -1;
                endless = true;
                extraPacmans = 127;

                GameController gc = new GameController(lives, attackDuration, endless);
                entities.add(new Pacman("pacman", gc, 'X', gc.getPositions('X')[0], 125));
                entities.add(new Ghost("blinky", gc, 'R', gc.getPositions('R')[0], 100, slowdownFactor, blinkSpeed));
                for (int i = 0; i < extraPacmans; i++) {
                    Pacman pc = new Pacman("Pacman nr: " + i,
                            gc,
                            'X',
                            gc.getPositions('X')[0],
                            ThreadLocalRandom.current().nextInt(80, 90 + 1));

                    entities.add(pc);
                }

                entities.forEach(gc::attachExtraEntity);
                entities.forEach(CThread::start);

                gc.waitingForGameToEnd();

                break;
            }
            default: {
                printHelp();
                System.exit(1);
            }
        }

        GameController gc = new GameController(lives, attackDuration, endless);


        entities.add(new Pacman("pacman", gc, 'X', gc.getPositions('X')[0], 125));

        // enemies
        entities.add(new Ghost("inky", gc, 'C', gc.getPositions('C')[0], 125, slowdownFactor, blinkSpeed));
        entities.add(new Ghost("pinky", gc, 'P', gc.getPositions('P')[0], 175, slowdownFactor, blinkSpeed));
        entities.add(new Ghost("clyde", gc, 'O', gc.getPositions('O')[0], 250, slowdownFactor, blinkSpeed));
        entities.add(new Ghost("blinky", gc, 'R', gc.getPositions('R')[0], 100, slowdownFactor, blinkSpeed));

        for (int i = 0; i < extraGhots; i++) {
            Point[] freeSlots = gc.getPositions('.');
            Point pos = freeSlots[ThreadLocalRandom.current().nextInt(0, freeSlots.length)];
            Ghost ghost = new Ghost("Generic Ghost " + i,
                    gc,
                    'G', // generic ghost
                    pos,
                    ThreadLocalRandom.current().nextInt(125, 500 + 1),
                    slowdownFactor,
                    blinkSpeed);

            entities.add(ghost);
        }

        for (int i = 0; i < extraPacmans; i++) {
            Pacman pc = new Pacman("Pacman nr: " + i,
                    gc,
                    'X',
                    gc.getPositions('X')[0],
                    ThreadLocalRandom.current().nextInt(80, 90 + 1));

            entities.add(pc);
        }

        entities.forEach(gc::attachExtraEntity);
        entities.forEach(CThread::start);

        bool won = gc.waitingForGameToEnd();
        LOG_PRINT("Game Ended! " + "Pacman as " + (won ? "won!" : "lost"));


    }

      void printHelp() {
        LOG_PRINT("PCOO Pacman Simulation 2016/2017");
        LOG_PRINT("Guilherme Cardoso <gjc@ua.pt>");
        LOG_PRINT("");
        LOG_PRINT("Usage: java -ea -jar Pacman.jar <preset>");
        LOG_PRINT("");
        LOG_PRINT("Available presets:");
        LOG_PRINT("");
        LOG_PRINT("1:  Normal pacman game. 3 lives, 4 ghosts.");
        LOG_PRINT("    Game whens when all points collected or no more lives");
        LOG_PRINT("");
        LOG_PRINT("2:  Aggressive mode: 100 lives, 16 ghosts");
        LOG_PRINT("    Attack duration lasts 15 seconds");
        LOG_PRINT("");
        LOG_PRINT("3:  Endless mode: infinite lives");
        LOG_PRINT("    Game doesn't end when all points are collected.");
        LOG_PRINT("");
        LOG_PRINT("4:  Crazy mode: infinite lives, 32 pacmans, 32 ghosts");
        LOG_PRINT("    Same as 3, but with more entities");
        LOG_PRINT("");
        LOG_PRINT("5:  Developer mode. 128 pacmans killing one Ghost. Tests interrupts and concurrency.");
    }

}
#endif
