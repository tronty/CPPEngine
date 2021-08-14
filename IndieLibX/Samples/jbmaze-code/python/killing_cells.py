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
import grid
import recursive_backtracker

grid = Grid(5, 5)

# orphan the cell in the northwest corner...
grid[0, 0].east.west = None 
grid[0, 0].south.north = None 

# ...and the one in the southeast corner
grid[4, 4].west.east = None 
grid[4, 4].north.south = None 

RecursiveBacktracker.on(grid, start_at=grid[1,1])

print grid
