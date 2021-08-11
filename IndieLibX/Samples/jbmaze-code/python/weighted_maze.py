#
# Copyright (c) 2018 Tommi Roenty   http://www.tommironty.fi/
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
import weighted_grid
import recursive_backtracker

grid = weighted_grid.WeightedGrid(10, 10)
recursive_backtracker.RecursiveBacktracker.on(grid)

# grid.braid 0.5 # ??? 
start, finish = grid[0, 0], grid[grid.rows - 1, grid.columns - 1]

grid.distances = start.distances.path_to(finish) 
filename = "original.png"
grid.saveImg(filename)
ln="saved to %s" % filename
print ln 

lava = grid.distances.cells.sample 
lava.weight = 50 

grid.distances = start.distances.path_to(finish) 
filename = "rerouted.png"
grid.saveImg(filename)
ln="saved to %s" % filename
print ln
