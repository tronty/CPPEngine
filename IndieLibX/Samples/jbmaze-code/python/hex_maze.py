#
# Copyright (c) 2018 Tuomas Roenty   http://www.tuomasronty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

#---
# Excerpted from "Mazes for Programmers",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/jbmaze for more book information.
#---
import recursive_backtracker
import hex_grid

grid = hex_grid.HexGrid(10, 10)
recursive_backtracker.RecursiveBacktracker.on(grid, grid.random_cell)

grid.saveImg(hex.png)
